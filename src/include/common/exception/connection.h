#pragma once

#include "common/api.h"
#include "exception.h"

namespace ryu {
namespace common {

class RYU_API ConnectionException : public Exception {
public:
    explicit ConnectionException(const std::string& msg)
        : Exception("Connection exception: " + msg){};
};

} // namespace common
} // namespace ryu
