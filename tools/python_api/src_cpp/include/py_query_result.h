#pragma once

#include <memory>
#include <vector>

#include "arrow_array.h"
#include "common/arrow/arrow.h"
#include "main/ryu.h"
#include "pybind_include.h"

using namespace ryu::main;

class PyQueryResult {
    friend class PyConnection;

public:
    static void initialize(py::handle& m);

    PyQueryResult() = default;

    ~PyQueryResult();

    bool hasNext();

    py::list getNext();

    bool hasNextQueryResult();

    std::unique_ptr<PyQueryResult> getNextQueryResult();

    void close();

    static py::object convertValueToPyObject(const ryu::common::Value& value);

    py::object getAsDF();

    ryu::pyarrow::Table getAsArrow(std::int64_t chunkSize, bool fallbackExtensionTypes);

    py::list getColumnDataTypes();

    py::list getColumnNames();

    void resetIterator();

    bool isSuccess() const;

    std::string getErrorMessage() const;

    double getExecutionTime();

    double getCompilingTime();

    size_t getNumTuples();

private:
    static py::dict convertNodeIdToPyDict(const ryu::common::nodeID_t& nodeId);

    void getNextArrowChunk(const std::vector<ryu::common::LogicalType>& types,
        const std::vector<std::string>& names, py::list& batches, std::int64_t chunkSize,
        bool fallbackExtensionTypes);
    py::object getArrowChunks(const std::vector<ryu::common::LogicalType>& types,
        const std::vector<std::string>& names, std::int64_t chunkSize, bool fallbackExtensionTypes);

private:
    QueryResult* queryResult = nullptr;
    bool isOwned = false;
};
