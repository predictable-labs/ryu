#include "storage/backup/backup_metadata.h"
#include "common/serializer/serializer.h"
#include "common/serializer/deserializer.h"
#include "common/file_system/virtual_file_system.h"
#include <fstream>
#include <chrono>

namespace ryu {
namespace storage {

void BackupMetadata::serialize(common::Serializer& serializer) const {
    serializer.write<common::transaction_t>(snapshotTS);
    serializer.writeString(databaseID);
    serializer.writeString(databasePath);
    serializer.write<uint64_t>(backupTimestamp);
    serializer.write<uint64_t>(numPages);
    serializer.write<uint64_t>(backupSizeBytes);
    serializer.writeString(ryuVersion);
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

void BackupMetadata::writeToFile(const std::string& metadataPath) const {
    auto fileSystem = common::VirtualFileSystem::getFileSystem();
    auto fileInfo = fileSystem->openFile(metadataPath,
        common::FileOpenFlags::WRITE | common::FileOpenFlags::CREATE_IF_NOT_EXISTS);

    common::Serializer serializer;
    serialize(serializer);

    auto data = serializer.getBuf();
    fileSystem->writeFile(*fileInfo, data.data(), data.size(), 0);
    fileSystem->closeFile(*fileInfo);
}

BackupMetadata BackupMetadata::readFromFile(const std::string& metadataPath) {
    auto fileSystem = common::VirtualFileSystem::getFileSystem();
    auto fileInfo = fileSystem->openFile(metadataPath, common::FileOpenFlags::READ_ONLY);

    auto fileSize = fileSystem->getFileSize(*fileInfo);
    std::vector<uint8_t> buffer(fileSize);
    fileSystem->readFile(*fileInfo, buffer.data(), fileSize, 0);
    fileSystem->closeFile(*fileInfo);

    common::Deserializer deserializer(buffer.data(), fileSize);
    return deserialize(deserializer);
}

} // namespace storage
} // namespace ryu
