#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common/api.h"

namespace ryu {
namespace common {

struct CaseInsensitiveStringHashFunction {
    RYU_API uint64_t operator()(const std::string& str) const;
};

struct CaseInsensitiveStringEquality {
    RYU_API bool operator()(const std::string& lhs, const std::string& rhs) const;
};

template<typename T>
using case_insensitive_map_t = std::unordered_map<std::string, T, CaseInsensitiveStringHashFunction,
    CaseInsensitiveStringEquality>;

using case_insensitve_set_t = std::unordered_set<std::string, CaseInsensitiveStringHashFunction,
    CaseInsensitiveStringEquality>;

} // namespace common
} // namespace ryu
