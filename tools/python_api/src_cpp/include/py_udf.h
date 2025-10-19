#pragma once

#include <string>

#include "common/types/types.h"
#include "function/function.h"
#include "pybind_include.h"

using ryu::common::LogicalTypeID;
using ryu::function::function_set;

namespace ryu {
namespace main {
class ClientContext;
} // namespace main
} // namespace ryu

class PyUDF {

public:
    static function_set toFunctionSet(const std::string& name, const py::function& udf,
        const py::list& paramTypes, const std::string& resultType, bool defaultNull,
        bool catchExceptions, ryu::main::ClientContext* context);
};
