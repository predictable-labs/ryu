#pragma once

#include <cstdint>

#include "common/api.h"

namespace ryu {
namespace common {

class Value;

class NestedVal {
public:
    RYU_API static uint32_t getChildrenSize(const Value* val);

    RYU_API static Value* getChildVal(const Value* val, uint32_t idx);
};

} // namespace common
} // namespace ryu
