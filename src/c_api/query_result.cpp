#include "main/query_result.h"

#include "c_api/helpers.h"
#include "c_api/ryu.h"

using namespace ryu::main;
using namespace ryu::common;
using namespace ryu::processor;

void ryu_query_result_destroy(ryu_query_result* query_result) {
    if (query_result == nullptr) {
        return;
    }
    if (query_result->_query_result != nullptr) {
        if (!query_result->_is_owned_by_cpp) {
            delete static_cast<QueryResult*>(query_result->_query_result);
        }
    }
}

bool ryu_query_result_is_success(ryu_query_result* query_result) {
    return static_cast<QueryResult*>(query_result->_query_result)->isSuccess();
}

char* ryu_query_result_get_error_message(ryu_query_result* query_result) {
    auto error_message = static_cast<QueryResult*>(query_result->_query_result)->getErrorMessage();
    if (error_message.empty()) {
        return nullptr;
    }
    return convertToOwnedCString(error_message);
}

uint64_t ryu_query_result_get_num_columns(ryu_query_result* query_result) {
    return static_cast<QueryResult*>(query_result->_query_result)->getNumColumns();
}

ryu_state ryu_query_result_get_column_name(ryu_query_result* query_result, uint64_t index,
    char** out_column_name) {
    auto column_names = static_cast<QueryResult*>(query_result->_query_result)->getColumnNames();
    if (index >= column_names.size()) {
        return RyuError;
    }
    *out_column_name = convertToOwnedCString(column_names[index]);
    return RyuSuccess;
}

ryu_state ryu_query_result_get_column_data_type(ryu_query_result* query_result, uint64_t index,
    ryu_logical_type* out_column_data_type) {
    auto column_data_types =
        static_cast<QueryResult*>(query_result->_query_result)->getColumnDataTypes();
    if (index >= column_data_types.size()) {
        return RyuError;
    }
    const auto& column_data_type = column_data_types[index];
    out_column_data_type->_data_type = new LogicalType(column_data_type.copy());
    return RyuSuccess;
}

uint64_t ryu_query_result_get_num_tuples(ryu_query_result* query_result) {
    return static_cast<QueryResult*>(query_result->_query_result)->getNumTuples();
}

ryu_state ryu_query_result_get_query_summary(ryu_query_result* query_result,
    ryu_query_summary* out_query_summary) {
    if (out_query_summary == nullptr) {
        return RyuError;
    }
    auto query_summary = static_cast<QueryResult*>(query_result->_query_result)->getQuerySummary();
    out_query_summary->_query_summary = query_summary;
    return RyuSuccess;
}

bool ryu_query_result_has_next(ryu_query_result* query_result) {
    return static_cast<QueryResult*>(query_result->_query_result)->hasNext();
}

bool ryu_query_result_has_next_query_result(ryu_query_result* query_result) {
    return static_cast<QueryResult*>(query_result->_query_result)->hasNextQueryResult();
}

ryu_state ryu_query_result_get_next_query_result(ryu_query_result* query_result,
    ryu_query_result* out_query_result) {
    if (!ryu_query_result_has_next_query_result(query_result)) {
        return RyuError;
    }
    auto next_query_result =
        static_cast<QueryResult*>(query_result->_query_result)->getNextQueryResult();
    if (next_query_result == nullptr) {
        return RyuError;
    }
    out_query_result->_query_result = next_query_result;
    out_query_result->_is_owned_by_cpp = true;
    return RyuSuccess;
}

ryu_state ryu_query_result_get_next(ryu_query_result* query_result,
    ryu_flat_tuple* out_flat_tuple) {
    try {
        auto flat_tuple = static_cast<QueryResult*>(query_result->_query_result)->getNext();
        out_flat_tuple->_flat_tuple = flat_tuple.get();
        out_flat_tuple->_is_owned_by_cpp = true;
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

char* ryu_query_result_to_string(ryu_query_result* query_result) {
    std::string result_string = static_cast<QueryResult*>(query_result->_query_result)->toString();
    return convertToOwnedCString(result_string);
}

void ryu_query_result_reset_iterator(ryu_query_result* query_result) {
    static_cast<QueryResult*>(query_result->_query_result)->resetIterator();
}

ryu_state ryu_query_result_get_arrow_schema(ryu_query_result* query_result,
    ArrowSchema* out_schema) {
    try {
        *out_schema = *static_cast<QueryResult*>(query_result->_query_result)->getArrowSchema();
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_query_result_get_next_arrow_chunk(ryu_query_result* query_result, int64_t chunk_size,
    ArrowArray* out_arrow_array) {
    try {
        *out_arrow_array =
            *static_cast<QueryResult*>(query_result->_query_result)->getNextArrowChunk(chunk_size);
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}
