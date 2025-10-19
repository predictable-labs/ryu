#pragma once

#include <cstdint>

namespace ryu::storage {
enum class CSRNodeGroupScanSource : uint8_t {
    COMMITTED_PERSISTENT = 0,
    COMMITTED_IN_MEMORY = 1,
    UNCOMMITTED = 2,
    NONE = 10
};
} // namespace ryu::storage
