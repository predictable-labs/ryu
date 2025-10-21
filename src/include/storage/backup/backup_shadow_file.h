#pragma once

#include "common/types/types.h"
#include "storage/buffer_manager/buffer_manager.h"
#include "storage/file_handle.h"
#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>

namespace ryu {
namespace storage {

class BufferManager;

/**
 * BackupShadowFile preserves original pages that are modified during backup.
 * Similar to checkpoint shadow file, but specifically for backup operations.
 */
class BackupShadowFile {
public:
    BackupShadowFile(const std::string& backupPath, BufferManager& bufferManager);
    ~BackupShadowFile();

    /**
     * Preserve original page before it's modified during backup.
     * This ensures the backup sees the consistent snapshot state.
     */
    void preserveOriginalPage(common::page_idx_t pageIdx, const uint8_t* pageData);

    /**
     * Read a preserved page from the shadow file.
     */
    void readPreservedPage(common::page_idx_t pageIdx, uint8_t* buffer);

    /**
     * Check if a page has been preserved in shadow file.
     */
    bool hasPreservedPage(common::page_idx_t pageIdx) const;

    /**
     * Get the number of pages preserved.
     */
    uint64_t getNumPreservedPages() const;

    /**
     * Close and delete the shadow file.
     */
    void cleanup();

private:
    std::string shadowFilePath;
    BufferManager& bufferManager;
    std::unique_ptr<FileHandle> shadowFileHandle;

    // Map from original page index to shadow page index
    std::unordered_map<common::page_idx_t, common::page_idx_t> pageMapping;
    mutable std::mutex mtx;

    // Counter for allocating shadow pages
    common::page_idx_t nextShadowPageIdx;
};

} // namespace storage
} // namespace ryu
