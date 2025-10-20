#pragma once

#include "common/api.h"
#include "exception.h"

namespace ryu {
namespace common {

class RYU_API StorageException : public Exception {
public:
    explicit StorageException(const std::string& msg) : Exception("Storage exception: " + msg){};
};

} // namespace common
} // namespace ryu
