#include "main/prepared_statement.h"

#include "c_api/helpers.h"
#include "c_api/ryu.h"
#include "common/types/value/value.h"

using namespace ryu::common;
using namespace ryu::main;

void ryu_prepared_statement_bind_cpp_value(ryu_prepared_statement* prepared_statement,
    const char* param_name, std::unique_ptr<Value> value) {
    auto* bound_values = static_cast<std::unordered_map<std::string, std::unique_ptr<Value>>*>(
        prepared_statement->_bound_values);
    bound_values->erase(param_name);
    bound_values->insert({param_name, std::move(value)});
}

void ryu_prepared_statement_destroy(ryu_prepared_statement* prepared_statement) {
    if (prepared_statement == nullptr) {
        return;
    }
    if (prepared_statement->_prepared_statement != nullptr) {
        delete static_cast<PreparedStatement*>(prepared_statement->_prepared_statement);
    }
    if (prepared_statement->_bound_values != nullptr) {
        delete static_cast<std::unordered_map<std::string, std::unique_ptr<Value>>*>(
            prepared_statement->_bound_values);
    }
}

bool ryu_prepared_statement_is_success(ryu_prepared_statement* prepared_statement) {
    return static_cast<PreparedStatement*>(prepared_statement->_prepared_statement)->isSuccess();
}

char* ryu_prepared_statement_get_error_message(ryu_prepared_statement* prepared_statement) {
    auto error_message =
        static_cast<PreparedStatement*>(prepared_statement->_prepared_statement)->getErrorMessage();
    if (error_message.empty()) {
        return nullptr;
    }
    return convertToOwnedCString(error_message);
}

ryu_state ryu_prepared_statement_bind_bool(ryu_prepared_statement* prepared_statement,
    const char* param_name, bool value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_int64(ryu_prepared_statement* prepared_statement,
    const char* param_name, int64_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_int32(ryu_prepared_statement* prepared_statement,
    const char* param_name, int32_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_int16(ryu_prepared_statement* prepared_statement,
    const char* param_name, int16_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_int8(ryu_prepared_statement* prepared_statement,
    const char* param_name, int8_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_uint64(ryu_prepared_statement* prepared_statement,
    const char* param_name, uint64_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_uint32(ryu_prepared_statement* prepared_statement,
    const char* param_name, uint32_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_uint16(ryu_prepared_statement* prepared_statement,
    const char* param_name, uint16_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_uint8(ryu_prepared_statement* prepared_statement,
    const char* param_name, uint8_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_double(ryu_prepared_statement* prepared_statement,
    const char* param_name, double value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_float(ryu_prepared_statement* prepared_statement,
    const char* param_name, float value) {
    try {
        auto value_ptr = std::make_unique<Value>(value);
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_date(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_date_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(date_t(value.days));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_timestamp_ns(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_timestamp_ns_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(timestamp_ns_t(value.value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_timestamp_ms(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_timestamp_ms_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(timestamp_ms_t(value.value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_timestamp_sec(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_timestamp_sec_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(timestamp_sec_t(value.value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_timestamp_tz(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_timestamp_tz_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(timestamp_tz_t(value.value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_timestamp(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_timestamp_t value) {
    try {
        auto value_ptr = std::make_unique<Value>(timestamp_t(value.value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_interval(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_interval_t value) {
    try {
        auto value_ptr =
            std::make_unique<Value>(interval_t(value.months, value.days, value.micros));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_string(ryu_prepared_statement* prepared_statement,
    const char* param_name, const char* value) {
    try {
        auto value_ptr = std::make_unique<Value>(std::string(value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_prepared_statement_bind_value(ryu_prepared_statement* prepared_statement,
    const char* param_name, ryu_value* value) {
    try {
        auto value_ptr = std::make_unique<Value>(*static_cast<Value*>(value->_value));
        ryu_prepared_statement_bind_cpp_value(prepared_statement, param_name, std::move(value_ptr));
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}
