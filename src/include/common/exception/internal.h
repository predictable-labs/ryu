#pragma once

#include "common/api.h"
#include "exception.h"

namespace ryu {
namespace common {

class RYU_API InternalException : public Exception {
public:
    explicit InternalException(const std::string& msg) : Exception(msg){};
};

} // namespace common
} // namespace ryu
