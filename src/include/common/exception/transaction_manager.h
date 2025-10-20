#pragma once

#include "common/api.h"
#include "exception.h"

namespace ryu {
namespace common {

class RYU_API TransactionManagerException : public Exception {
public:
    explicit TransactionManagerException(const std::string& msg) : Exception(msg){};
};

} // namespace common
} // namespace ryu
