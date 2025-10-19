#pragma once
#include <cstdint>

#include "common/api.h"
namespace ryu {
namespace main {

struct Version {
public:
    /**
     * @brief Get the version of the Ryu library.
     * @return const char* The version of the Ryu library.
     */
    RYU_API static const char* getVersion();

    /**
     * @brief Get the storage version of the Ryu library.
     * @return uint64_t The storage version of the Ryu library.
     */
    RYU_API static uint64_t getStorageVersion();
};
} // namespace main
} // namespace ryu
