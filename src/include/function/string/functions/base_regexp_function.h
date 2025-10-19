#pragma once

#include <regex>

#include "common/vector/value_vector.h"

namespace ryu {
namespace function {

struct BaseRegexpOperation {
    static inline std::string parseCypherPattern(const std::string& pattern) {
        // Cypher parses escape characters with 2 backslash eg. for expressing '.' requires '\\.'
        // Since Regular Expression requires only 1 backslash '\.' we need to replace double slash
        // with single
        return std::regex_replace(pattern, std::regex(R"(\\\\)"), "\\");
    }

    static inline void copyToRyuString(const std::string& value, common::ku_string_t& ryString,
        common::ValueVector& valueVector) {
        common::StringVector::addString(&valueVector, ryString, value.data(), value.length());
    }
};

} // namespace function
} // namespace ryu
