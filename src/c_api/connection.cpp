#include "c_api/ryu.h"
#include "common/exception/exception.h"
#include "main/ryu.h"

namespace ryu {
namespace common {
class Value;
}
} // namespace ryu

using namespace ryu::common;
using namespace ryu::main;

ryu_state ryu_connection_init(ryu_database* database, ryu_connection* out_connection) {
    if (database == nullptr || database->_database == nullptr) {
        out_connection->_connection = nullptr;
        return RyuError;
    }
    try {
        out_connection->_connection = new Connection(static_cast<Database*>(database->_database));
    } catch (Exception& e) {
        out_connection->_connection = nullptr;
        return RyuError;
    }
    return RyuSuccess;
}

void ryu_connection_destroy(ryu_connection* connection) {
    if (connection == nullptr) {
        return;
    }
    if (connection->_connection != nullptr) {
        delete static_cast<Connection*>(connection->_connection);
    }
}

ryu_state ryu_connection_set_max_num_thread_for_exec(ryu_connection* connection,
    uint64_t num_threads) {
    if (connection == nullptr || connection->_connection == nullptr) {
        return RyuError;
    }
    try {
        static_cast<Connection*>(connection->_connection)->setMaxNumThreadForExec(num_threads);
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_connection_get_max_num_thread_for_exec(ryu_connection* connection,
    uint64_t* out_result) {
    if (connection == nullptr || connection->_connection == nullptr) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Connection*>(connection->_connection)->getMaxNumThreadForExec();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_connection_query(ryu_connection* connection, const char* query,
    ryu_query_result* out_query_result) {
    if (connection == nullptr || connection->_connection == nullptr) {
        return RyuError;
    }
    try {
        auto query_result =
            static_cast<Connection*>(connection->_connection)->query(query).release();
        if (query_result == nullptr) {
            return RyuError;
        }
        out_query_result->_query_result = query_result;
        out_query_result->_is_owned_by_cpp = false;
        if (!query_result->isSuccess()) {
            return RyuError;
        }
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_connection_prepare(ryu_connection* connection, const char* query,
    ryu_prepared_statement* out_prepared_statement) {
    if (connection == nullptr || connection->_connection == nullptr) {
        return RyuError;
    }
    try {
        auto prepared_statement =
            static_cast<Connection*>(connection->_connection)->prepare(query).release();
        if (prepared_statement == nullptr) {
            return RyuError;
        }
        out_prepared_statement->_prepared_statement = prepared_statement;
        out_prepared_statement->_bound_values =
            new std::unordered_map<std::string, std::unique_ptr<Value>>;
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_connection_execute(ryu_connection* connection,
    ryu_prepared_statement* prepared_statement, ryu_query_result* out_query_result) {
    if (connection == nullptr || connection->_connection == nullptr ||
        prepared_statement == nullptr || prepared_statement->_prepared_statement == nullptr ||
        prepared_statement->_bound_values == nullptr) {
        return RyuError;
    }
    try {
        auto prepared_statement_ptr =
            static_cast<PreparedStatement*>(prepared_statement->_prepared_statement);
        auto bound_values = static_cast<std::unordered_map<std::string, std::unique_ptr<Value>>*>(
            prepared_statement->_bound_values);

        // Must copy the parameters for safety, and so that the parameters in the prepared statement
        // stay the same.
        std::unordered_map<std::string, std::unique_ptr<Value>> copied_bound_values;
        for (auto& [name, value] : *bound_values) {
            copied_bound_values.emplace(name, value->copy());
        }

        auto query_result =
            static_cast<Connection*>(connection->_connection)
                ->executeWithParams(prepared_statement_ptr, std::move(copied_bound_values))
                .release();
        if (query_result == nullptr) {
            return RyuError;
        }
        out_query_result->_query_result = query_result;
        out_query_result->_is_owned_by_cpp = false;
        if (!query_result->isSuccess()) {
            return RyuError;
        }
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}
void ryu_connection_interrupt(ryu_connection* connection) {
    static_cast<Connection*>(connection->_connection)->interrupt();
}

ryu_state ryu_connection_set_query_timeout(ryu_connection* connection, uint64_t timeout_in_ms) {
    if (connection == nullptr || connection->_connection == nullptr) {
        return RyuError;
    }
    try {
        static_cast<Connection*>(connection->_connection)->setQueryTimeOut(timeout_in_ms);
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}
