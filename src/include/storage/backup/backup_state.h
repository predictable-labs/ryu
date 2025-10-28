#pragma once

namespace ryu {
namespace storage {

enum class BackupState : uint8_t {
    IDLE = 0,
    IN_PROGRESS = 1,
    FINALIZING = 2,
    COMPLETED = 3,
    FAILED = 4
};

} // namespace storage
} // namespace ryu
