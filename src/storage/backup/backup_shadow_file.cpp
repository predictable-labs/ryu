#include "storage/backup/backup_shadow_file.h"
#include "storage/buffer_manager/buffer_manager.h"
#include "storage/file_handle.h"
#include "common/file_system/virtual_file_system.h"
#include "common/exception/exception.h"
#include <cstring>

namespace ryu {
namespace storage {

BackupShadowFile::BackupShadowFile(const std::string& backupPath, BufferManager& bufferManager)
    : shadowFilePath(backupPath + ".shadow"),
      bufferManager(bufferManager),
      nextShadowPageIdx(0) {

    auto fileSystem = common::VirtualFileSystem::getFileSystem();

    // Create shadow file
    auto fileInfo = fileSystem->openFile(shadowFilePath,
        common::FileOpenFlags::WRITE |
        common::FileOpenFlags::CREATE_IF_NOT_EXISTS);

    // Create FileHandle for shadow file
    shadowFileHandle = std::make_unique<FileHandle>(
        shadowFilePath,
        common::FileOpenFlags::WRITE | common::FileOpenFlags::READ_ONLY,
        &bufferManager);
}

BackupShadowFile::~BackupShadowFile() {
    cleanup();
}

void BackupShadowFile::preserveOriginalPage(common::page_idx_t pageIdx, const uint8_t* pageData) {
    std::lock_guard<std::mutex> lock(mtx);

    // Check if already preserved
    if (pageMapping.find(pageIdx) != pageMapping.end()) {
        return;  // Already preserved
    }

    // Allocate new shadow page
    common::page_idx_t shadowPageIdx = nextShadowPageIdx++;

    // Write page data to shadow file
    auto shadowPage = bufferManager.pin(*shadowFileHandle, shadowPageIdx,
                                        common::PageReadPolicy::WRITE_PAGE);
    std::memcpy(shadowPage, pageData, common::BufferPoolConstants::PAGE_4KB_SIZE);
    bufferManager.unpin(*shadowFileHandle, shadowPageIdx);

    // Record mapping
    pageMapping[pageIdx] = shadowPageIdx;
}

void BackupShadowFile::readPreservedPage(common::page_idx_t pageIdx, uint8_t* buffer) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = pageMapping.find(pageIdx);
    if (it == pageMapping.end()) {
        throw common::Exception("Page " + std::to_string(pageIdx) +
                                " not found in backup shadow file");
    }

    common::page_idx_t shadowPageIdx = it->second;

    // Read from shadow file
    auto shadowPage = bufferManager.pin(*shadowFileHandle, shadowPageIdx,
                                        common::PageReadPolicy::READ_ONLY);
    std::memcpy(buffer, shadowPage, common::BufferPoolConstants::PAGE_4KB_SIZE);
    bufferManager.unpin(*shadowFileHandle, shadowPageIdx);
}

bool BackupShadowFile::hasPreservedPage(common::page_idx_t pageIdx) const {
    std::lock_guard<std::mutex> lock(mtx);
    return pageMapping.find(pageIdx) != pageMapping.end();
}

uint64_t BackupShadowFile::getNumPreservedPages() const {
    std::lock_guard<std::mutex> lock(mtx);
    return pageMapping.size();
}

void BackupShadowFile::cleanup() {
    if (shadowFileHandle) {
        shadowFileHandle.reset();

        // Delete shadow file
        auto fileSystem = common::VirtualFileSystem::getFileSystem();
        if (fileSystem->fileOrPathExists(shadowFilePath)) {
            fileSystem->removeFileIfExists(shadowFilePath);
        }
    }
}

} // namespace storage
} // namespace ryu
