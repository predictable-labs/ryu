#include "storage/backup/backup_shadow_file.h"
#include "storage/buffer_manager/buffer_manager.h"
#include "common/constants.h"
#include "common/exception/exception.h"
#include <cstring>

namespace ryu {
namespace storage {

BackupShadowFile::BackupShadowFile(const std::string& backupPath, BufferManager& bufferManager)
    : shadowFilePath(backupPath + ".shadow"),
      bufferManager(bufferManager),
      nextShadowPageIdx(0) {
    // Shadow file stores pages in memory temporarily
    // No need to create actual file handle
}

BackupShadowFile::~BackupShadowFile() {
    cleanup();
}

void BackupShadowFile::preserveOriginalPage(common::page_idx_t pageIdx, const uint8_t* pageData) {
    std::lock_guard<std::mutex> lock(mtx);

    // Check if already preserved
    if (shadowPages.find(pageIdx) != shadowPages.end()) {
        return;  // Already preserved
    }

    // Allocate and copy page data to in-memory buffer
    std::vector<uint8_t> pageCopy(common::RYU_PAGE_SIZE);
    std::memcpy(pageCopy.data(), pageData, common::RYU_PAGE_SIZE);

    // Store in memory
    shadowPages[pageIdx] = std::move(pageCopy);
}

void BackupShadowFile::readPreservedPage(common::page_idx_t pageIdx, uint8_t* buffer) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = shadowPages.find(pageIdx);
    if (it == shadowPages.end()) {
        throw common::Exception("Page " + std::to_string(pageIdx) +
                                " not found in backup shadow file");
    }

    // Copy from in-memory buffer
    std::memcpy(buffer, it->second.data(), common::RYU_PAGE_SIZE);
}

bool BackupShadowFile::hasPreservedPage(common::page_idx_t pageIdx) const {
    std::lock_guard<std::mutex> lock(mtx);
    return shadowPages.find(pageIdx) != shadowPages.end();
}

uint64_t BackupShadowFile::getNumPreservedPages() const {
    std::lock_guard<std::mutex> lock(mtx);
    return shadowPages.size();
}

void BackupShadowFile::cleanup() {
    std::lock_guard<std::mutex> lock(mtx);
    // Clear all in-memory shadow pages
    shadowPages.clear();
}

} // namespace storage
} // namespace ryu
