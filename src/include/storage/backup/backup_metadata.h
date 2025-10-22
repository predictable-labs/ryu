#pragma once

#include "common/types/types.h"
#include "common/serializer/deserializer.h"
#include "common/serializer/serializer.h"
#include <string>
#include <cstdint>

namespace ryu {
namespace common {
class VirtualFileSystem;
}

namespace storage {

struct BackupMetadata {
    common::transaction_t snapshotTS;     // Snapshot timestamp
    std::string databaseID;                // Database ID (UUID as string)
    std::string databasePath;              // Original DB path
    uint64_t backupTimestamp;              // Unix timestamp
    uint64_t numPages;                     // Total pages backed up
    uint64_t backupSizeBytes;              // Total size in bytes
    std::string ryuVersion;                // Ryu version string

    // Future: Raft-specific fields (optional)
    // std::optional<uint64_t> raftLogIndex;
    // std::optional<std::string> nodeRole;
    // std::optional<std::string> clusterId;

    void serialize(common::Serializer& serializer) const;
    static BackupMetadata deserialize(common::Deserializer& deserializer);

    // Write metadata to file
    void writeToFile(const std::string& metadataPath, common::VirtualFileSystem* vfs) const;

    // Read metadata from file
    static BackupMetadata readFromFile(const std::string& metadataPath, common::VirtualFileSystem* vfs);
};

} // namespace storage
} // namespace ryu
