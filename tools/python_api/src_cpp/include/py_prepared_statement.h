#pragma once

#include "main/prepared_statement.h"
#include "main/ryu.h"
#include "pybind_include.h"

using namespace ryu::main;

class PyPreparedStatement {
    friend class PyConnection;

public:
    static void initialize(py::handle& m);

    py::str getErrorMessage() const;

    bool isSuccess() const;

private:
    std::unique_ptr<PreparedStatement> preparedStatement;
};
