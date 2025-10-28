#include "storage/backup/backup_metadata.h"
#include "common/serializer/serializer.h"
#include "common/serializer/deserializer.h"
#include "common/serializer/buffered_file.h"
#include "common/file_system/virtual_file_system.h"

namespace ryu {
namespace storage {

void BackupMetadata::serialize(common::Serializer& serializer) const {
    serializer.write<common::transaction_t>(snapshotTS);
    serializer.serializeValue<std::string>(databaseID);
    serializer.serializeValue<std::string>(databasePath);
    serializer.write<uint64_t>(backupTimestamp);
    serializer.write<uint64_t>(numPages);
    serializer.write<uint64_t>(backupSizeBytes);
    serializer.serializeValue<std::string>(ryuVersion);
}

BackupMetadata BackupMetadata::deserialize(common::Deserializer& deserializer) {
    BackupMetadata metadata;
    deserializer.deserializeValue<common::transaction_t>(metadata.snapshotTS);
    deserializer.deserializeValue<std::string>(metadata.databaseID);
    deserializer.deserializeValue<std::string>(metadata.databasePath);
    deserializer.deserializeValue<uint64_t>(metadata.backupTimestamp);
    deserializer.deserializeValue<uint64_t>(metadata.numPages);
    deserializer.deserializeValue<uint64_t>(metadata.backupSizeBytes);
    deserializer.deserializeValue<std::string>(metadata.ryuVersion);
    return metadata;
}

void BackupMetadata::writeToFile(const std::string& metadataPath, common::VirtualFileSystem* vfs) const {
    // TODO: Implement once we understand proper VFS API
    // For now, this is a placeholder
}

BackupMetadata BackupMetadata::readFromFile(const std::string& metadataPath, common::VirtualFileSystem* vfs) {
    // TODO: Implement once we understand proper VFS API
    // For now, this is a placeholder
    return BackupMetadata{};
}

} // namespace storage
} // namespace ryu
