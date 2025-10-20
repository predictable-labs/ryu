#pragma once

#include "common/api.h"
#include "exception.h"

namespace ryu {
namespace common {

class RYU_API BufferManagerException : public Exception {
public:
    explicit BufferManagerException(const std::string& msg)
        : Exception("Buffer manager exception: " + msg){};
};

} // namespace common
} // namespace ryu
