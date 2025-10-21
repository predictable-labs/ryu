#pragma once

#include "common/types/types.h"
#include "storage/backup/backup_state.h"
#include "storage/backup/backup_metadata.h"
#include "storage/backup/backup_shadow_file.h"
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_set>
#include <condition_variable>

namespace ryu {

namespace main {
class ClientContext;
class Database;
}

namespace storage {

class StorageManager;
class BufferManager;
class FileHandle;

/**
 * BackupManager implements zero-downtime backup for Ryu database.
 *
 * Key features:
 * - Background thread for backup (non-blocking)
 * - Snapshot isolation (point-in-time consistency)
 * - Copy-on-write for pages modified during backup
 * - Progress tracking
 * - Backup verification
 */
class BackupManager {
public:
    explicit BackupManager(main::Database* database);
    ~BackupManager();

    // Disable copy and move
    BackupManager(const BackupManager&) = delete;
    BackupManager& operator=(const BackupManager&) = delete;
    BackupManager(BackupManager&&) = delete;
    BackupManager& operator=(BackupManager&&) = delete;

    /**
     * Start backup to the specified path.
     * Returns immediately; backup runs in background thread.
     */
    void startBackup(const std::string& backupPath);

    /**
     * Wait for backup to complete (blocking).
     */
    void waitForCompletion();

    /**
     * Get current backup state.
     */
    BackupState getBackupState() const;

    /**
     * Get backup progress (0.0 to 1.0).
     */
    double getBackupProgress() const;

    /**
     * Cancel ongoing backup.
     */
    void cancelBackup();

    /**
     * Called by BufferManager when a page is about to be modified.
     * Preserves original page if backup is in progress and page not yet copied.
     */
    void notifyPageModification(common::page_idx_t pageIdx);

    /**
     * Restore database from backup.
     * Static method that can be called without active database.
     */
    static void restoreFromBackup(const std::string& backupPath,
                                   const std::string& targetDbPath);

private:
    // Background thread function
    void backupThreadFunc();

    // Core backup operations
    void copyMainDataFile();
    void copyWALFile();
    void copyMetadata();
    void writeBackupMetadata();
    bool verifyBackupIntegrity();

    // Helper methods
    void copyPageWithSnapshot(common::page_idx_t pageIdx);
    bool isPageModifiedAfterSnapshot(common::page_idx_t pageIdx) const;
    void updateProgress(uint64_t currentPage, uint64_t totalPages);
    void setState(BackupState newState);
    void handleBackupError(const std::string& errorMsg);

    // Verification helpers
    bool verifyPageChecksum(common::page_idx_t pageIdx);
    bool verifyBackupMetadata();
    uint64_t countBackupPages();

    // Static restore helpers
    static void copyBackupToTarget(const std::string& backupPath,
                                    const std::string& targetDbPath);
    static void verifyRestoreIntegrity(const std::string& targetDbPath);

private:
    main::Database* database;
    std::string backupPath;
    std::string databasePath;

    // Backup state
    std::atomic<BackupState> state;
    std::atomic<double> progress;
    std::atomic<bool> cancelRequested;

    // Snapshot information
    common::transaction_t snapshotTS;

    // Background thread
    std::unique_ptr<std::thread> backupThread;
    std::mutex mtx;
    std::condition_variable cv;

    // Track which pages have been copied
    std::unordered_set<common::page_idx_t> copiedPages;

    // Shadow file for modified pages during backup
    std::unique_ptr<BackupShadowFile> backupShadowFile;

    // Backup metadata
    BackupMetadata metadata;

    // Error message if backup failed
    std::string errorMessage;
};

} // namespace storage
} // namespace ryu
