#include "storage/backup/backup_manager.h"
#include "main/database.h"
#include "storage/storage_manager.h"
#include "storage/file_handle.h"
#include "storage/wal/wal.h"
#include "storage/storage_utils.h"
#include "common/file_system/virtual_file_system.h"
#include "common/exception/exception.h"
#include "common/constants.h"
#include "common/string_format.h"
#include <chrono>
#include <thread>
#include <filesystem>

namespace ryu {
namespace storage {

BackupManager::BackupManager(main::Database* database)
    : database(database),
      state(BackupState::IDLE),
      progress(0.0),
      cancelRequested(false),
      snapshotTS(0) {

    if (!database) {
        throw common::Exception("Database cannot be null for BackupManager");
    }

    databasePath = database->getDatabasePath();
}

BackupManager::~BackupManager() {
    // Cancel any ongoing backup
    if (state.load() == BackupState::IN_PROGRESS) {
        cancelBackup();
    }

    // Wait for backup thread to finish
    if (backupThread && backupThread->joinable()) {
        backupThread->join();
    }
}

void BackupManager::startBackup(const std::string& backupPath) {
    std::lock_guard<std::mutex> lock(mtx);

    // Check if backup already in progress
    if (state.load() != BackupState::IDLE) {
        throw common::Exception("Backup already in progress or not idle");
    }

    this->backupPath = backupPath;
    this->cancelRequested.store(false);
    this->progress.store(0.0);
    this->errorMessage.clear();

    // Create backup directory
    auto vfs = database->getVFS();
    if (!vfs->fileOrPathExists(backupPath)) {
        vfs->createDir(backupPath);
    }

    // Get snapshot timestamp - use current time as placeholder
    // TODO: Get from transaction manager when API is available
    snapshotTS = std::chrono::system_clock::now().time_since_epoch().count();

    // Initialize backup metadata
    metadata.snapshotTS = snapshotTS;
    metadata.databaseID = "ryu-db";  // TODO: Get from database header
    metadata.databasePath = databasePath;
    metadata.backupTimestamp = std::chrono::system_clock::now().time_since_epoch().count();
    metadata.ryuVersion = common::RYU_VERSION;
    metadata.numPages = 0;
    metadata.backupSizeBytes = 0;

    // Clear copied pages set
    copiedPages.clear();

    // Start backup thread
    setState(BackupState::IN_PROGRESS);
    backupThread = std::make_unique<std::thread>(&BackupManager::backupThreadFunc, this);
}

void BackupManager::waitForCompletion() {
    if (backupThread && backupThread->joinable()) {
        backupThread->join();
    }
}

BackupState BackupManager::getBackupState() const {
    return state.load();
}

double BackupManager::getBackupProgress() const {
    return progress.load();
}

void BackupManager::cancelBackup() {
    cancelRequested.store(true);
    cv.notify_all();
}

void BackupManager::notifyPageModification(common::page_idx_t pageIdx) {
    std::lock_guard<std::mutex> lock(mtx);

    // Only preserve if backup is in progress
    if (state.load() != BackupState::IN_PROGRESS) {
        return;
    }

    // If page hasn't been copied yet, save it to shadow file
    if (copiedPages.find(pageIdx) == copiedPages.end()) {
        if (backupShadowFile) {
            // Mark as needing preservation
            // The shadow file will preserve this page
            copiedPages.insert(pageIdx);
        }
    }
}

void BackupManager::backupThreadFunc() {
    try {
        // Step 1: Copy main data file (70% of progress)
        copyMainDataFile();
        if (cancelRequested.load()) {
            setState(BackupState::FAILED);
            errorMessage = "Backup cancelled by user";
            return;
        }

        // Step 2: Copy WAL file (20% of progress)
        copyWALFile();
        if (cancelRequested.load()) {
            setState(BackupState::FAILED);
            errorMessage = "Backup cancelled by user";
            return;
        }

        // Step 3: Write backup metadata (10% of progress)
        writeBackupMetadata();
        if (cancelRequested.load()) {
            setState(BackupState::FAILED);
            errorMessage = "Backup cancelled by user";
            return;
        }

        // Backup completed successfully
        progress.store(1.0);
        setState(BackupState::COMPLETED);

    } catch (const std::exception& e) {
        handleBackupError(e.what());
    }
}

void BackupManager::copyMainDataFile() {
    auto storageManager = database->getStorageManager();
    auto vfs = database->getVFS();
    auto dataFH = storageManager->getDataFH();

    if (!dataFH) {
        throw common::Exception("Data file handle is null");
    }

    auto fileInfo = dataFH->getFileInfo();
    if (!fileInfo) {
        throw common::Exception("File info is null");
    }

    // Get file path from the database path
    std::string sourceDataFile = databasePath;
    std::string destDataFile = common::FileSystem::joinPath(backupPath,
        std::filesystem::path(databasePath).filename().string());

    // Copy file using VFS
    try {
        vfs->copyFile(sourceDataFile, destDataFile);

        // Update metadata
        auto numPages = dataFH->getNumPages();
        metadata.numPages = numPages;

        // Estimate file size
        metadata.backupSizeBytes = numPages * dataFH->getPageSize();

        // Update progress (70% for main data file)
        progress.store(0.7);

    } catch (const std::exception& e) {
        throw common::Exception(common::stringFormat(
            "Failed to copy main data file: {}", e.what()));
    }
}

void BackupManager::copyWALFile() {
    auto vfs = database->getVFS();

    // Construct WAL file path
    std::string walPath = storage::StorageUtils::getWALFilePath(databasePath);

    // Check if WAL file exists
    if (!vfs->fileOrPathExists(walPath)) {
        // No WAL file, skip
        progress.store(0.9);
        return;
    }

    try {
        // Get WAL destination path
        std::string walFileName = std::filesystem::path(walPath).filename().string();
        std::string destWalFile = common::FileSystem::joinPath(backupPath, walFileName);

        // Copy WAL file
        vfs->copyFile(walPath, destWalFile);

        // Update progress (20% for WAL file)
        progress.store(0.9);

    } catch (const std::exception& e) {
        throw common::Exception(common::stringFormat(
            "Failed to copy WAL file: {}", e.what()));
    }
}

void BackupManager::copyMetadata() {
    // This is called as part of writeBackupMetadata
}

void BackupManager::writeBackupMetadata() {
    try {
        // Create metadata file path
        std::string metadataPath = common::FileSystem::joinPath(backupPath, "backup_metadata.json");

        // For now, write a simple text file with metadata
        // TODO: Implement proper serialization using BackupMetadata::writeToFile
        auto vfs = database->getVFS();

        // Create a simple metadata content
        std::string metadataContent = common::stringFormat(
            "Ryu Database Backup Metadata\n"
            "============================\n"
            "Snapshot TS: {}\n"
            "Database ID: {}\n"
            "Database Path: {}\n"
            "Backup Timestamp: {}\n"
            "Ryu Version: {}\n"
            "Num Pages: {}\n"
            "Backup Size (bytes): {}\n",
            metadata.snapshotTS,
            metadata.databaseID,
            metadata.databasePath,
            metadata.backupTimestamp,
            metadata.ryuVersion,
            metadata.numPages,
            metadata.backupSizeBytes
        );

        // Write metadata using FileSystem
        auto metadataFileInfo = vfs->openFile(metadataPath,
            common::FileOpenFlags(common::FileFlags::WRITE |
                                 common::FileFlags::CREATE_AND_TRUNCATE_IF_EXISTS));

        metadataFileInfo->writeFile(
            reinterpret_cast<const uint8_t*>(metadataContent.c_str()),
            metadataContent.size(),
            0
        );

        // Update final progress
        progress.store(1.0);

    } catch (const std::exception& e) {
        throw common::Exception(common::stringFormat(
            "Failed to write backup metadata: {}", e.what()));
    }
}

bool BackupManager::verifyBackupIntegrity() {
    // TODO: Implement backup verification
    return true;
}

void BackupManager::copyPageWithSnapshot(common::page_idx_t pageIdx) {
    // TODO: Implement page-level copy with snapshot isolation
}

bool BackupManager::isPageModifiedAfterSnapshot(common::page_idx_t pageIdx) const {
    // TODO: Implement timestamp-based checking
    return false;
}

void BackupManager::updateProgress(uint64_t currentPage, uint64_t totalPages) {
    if (totalPages > 0) {
        progress.store(static_cast<double>(currentPage) / totalPages);
    }
}

void BackupManager::setState(BackupState newState) {
    state.store(newState);
    cv.notify_all();
}

void BackupManager::handleBackupError(const std::string& errorMsg) {
    setState(BackupState::FAILED);
    errorMessage = errorMsg;
}

bool BackupManager::verifyPageChecksum(common::page_idx_t pageIdx) {
    // TODO: Implement checksum verification
    return true;
}

bool BackupManager::verifyBackupMetadata() {
    // TODO: Implement metadata verification
    return true;
}

uint64_t BackupManager::countBackupPages() {
    return metadata.numPages;
}

void BackupManager::restoreFromBackup(const std::string& backupPath,
                                       const std::string& targetDbPath) {
    // Validate backup directory exists
    namespace fs = std::filesystem;
    if (!fs::exists(backupPath)) {
        throw common::Exception(common::stringFormat(
            "Backup directory does not exist: {}", backupPath));
    }

    // Validate target doesn't exist or is empty
    if (fs::exists(targetDbPath)) {
        throw common::Exception(common::stringFormat(
            "Target database path already exists: {}", targetDbPath));
    }

    try {
        // Create parent directory if needed
        auto parentPath = fs::path(targetDbPath).parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            fs::create_directories(parentPath);
        }

        // Copy all files from backup to target
        copyBackupToTarget(backupPath, targetDbPath);

        // Verify restore integrity
        verifyRestoreIntegrity(targetDbPath);

    } catch (const std::exception& e) {
        throw common::Exception(common::stringFormat(
            "Failed to restore from backup: {}", e.what()));
    }
}

void BackupManager::copyBackupToTarget(const std::string& backupPath,
                                        const std::string& targetDbPath) {
    namespace fs = std::filesystem;

    // Find the main database file in backup
    for (const auto& entry : fs::directory_iterator(backupPath)) {
        std::string filename = entry.path().filename().string();

        // Skip metadata files
        if (filename == "backup_metadata.json" || filename.find("shadow") != std::string::npos) {
            continue;
        }

        // Copy database files
        std::string sourcePath = entry.path().string();
        std::string targetPath;

        // Reconstruct target paths
        if (filename.find(".wal") != std::string::npos) {
            targetPath = storage::StorageUtils::getWALFilePath(targetDbPath);
        } else {
            targetPath = targetDbPath;
        }

        fs::copy_file(sourcePath, targetPath, fs::copy_options::overwrite_existing);
    }
}

void BackupManager::verifyRestoreIntegrity(const std::string& targetDbPath) {
    // TODO: Implement verification of restored database
    namespace fs = std::filesystem;

    // Basic check: ensure main database file exists
    if (!fs::exists(targetDbPath)) {
        throw common::Exception(common::stringFormat(
            "Restore verification failed: target database file not found: {}", targetDbPath));
    }
}

} // namespace storage
} // namespace ryu
