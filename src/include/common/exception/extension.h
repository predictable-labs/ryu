#pragma once

#include "exception.h"

namespace ryu {
namespace common {

class RYU_API ExtensionException : public Exception {
public:
    explicit ExtensionException(const std::string& msg)
        : Exception("Extension exception: " + msg) {}
};

} // namespace common
} // namespace ryu
