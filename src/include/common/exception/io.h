#pragma once

#include "exception.h"

namespace ryu {
namespace common {

class RYU_API IOException : public Exception {
public:
    explicit IOException(const std::string& msg) : Exception("IO exception: " + msg) {}
};

} // namespace common
} // namespace ryu
