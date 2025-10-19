#include "common/types/value/value.h"

#include "c_api/helpers.h"
#include "c_api/ryu.h"
#include "common/constants.h"
#include "common/types/types.h"
#include "common/types/value/nested.h"
#include "common/types/value/node.h"
#include "common/types/value/recursive_rel.h"
#include "common/types/value/rel.h"
#include "function/cast/functions/cast_from_string_functions.h"

using namespace ryu::common;

ryu_value* ryu_value_create_null() {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(Value::createNullValue());
    return c_value;
}

ryu_value* ryu_value_create_null_with_data_type(ryu_logical_type* data_type) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value =
        new Value(Value::createNullValue(*static_cast<LogicalType*>(data_type->_data_type)));
    return c_value;
}

bool ryu_value_is_null(ryu_value* value) {
    return static_cast<Value*>(value->_value)->isNull();
}

void ryu_value_set_null(ryu_value* value, bool is_null) {
    static_cast<Value*>(value->_value)->setNull(is_null);
}

ryu_value* ryu_value_create_default(ryu_logical_type* data_type) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value =
        new Value(Value::createDefaultValue(*static_cast<LogicalType*>(data_type->_data_type)));
    return c_value;
}

ryu_value* ryu_value_create_bool(bool val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_int8(int8_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_int16(int16_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_int32(int32_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_int64(int64_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_uint8(uint8_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_uint16(uint16_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_uint32(uint32_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_uint64(uint64_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_int128(ryu_int128_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    int128_t int128(val_.low, val_.high);
    c_value->_value = new Value(int128);
    return c_value;
}

ryu_value* ryu_value_create_float(float val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_double(double val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_value* ryu_value_create_internal_id(ryu_internal_id_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    internalID_t id(val_.offset, val_.table_id);
    c_value->_value = new Value(id);
    return c_value;
}

ryu_value* ryu_value_create_date(ryu_date_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto date = date_t(val_.days);
    c_value->_value = new Value(date);
    return c_value;
}

ryu_value* ryu_value_create_timestamp_ns(ryu_timestamp_ns_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto timestamp_ns = timestamp_ns_t(val_.value);
    c_value->_value = new Value(timestamp_ns);
    return c_value;
}

ryu_value* ryu_value_create_timestamp_ms(ryu_timestamp_ms_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto timestamp_ms = timestamp_ms_t(val_.value);
    c_value->_value = new Value(timestamp_ms);
    return c_value;
}

ryu_value* ryu_value_create_timestamp_sec(ryu_timestamp_sec_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto timestamp_sec = timestamp_sec_t(val_.value);
    c_value->_value = new Value(timestamp_sec);
    return c_value;
}

ryu_value* ryu_value_create_timestamp_tz(ryu_timestamp_tz_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto timestamp_tz = timestamp_tz_t(val_.value);
    c_value->_value = new Value(timestamp_tz);
    return c_value;
}

ryu_value* ryu_value_create_timestamp(ryu_timestamp_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto timestamp = timestamp_t(val_.value);
    c_value->_value = new Value(timestamp);
    return c_value;
}

ryu_value* ryu_value_create_interval(ryu_interval_t val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    auto interval = interval_t(val_.months, val_.days, val_.micros);
    c_value->_value = new Value(interval);
    return c_value;
}

ryu_value* ryu_value_create_string(const char* val_) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(val_);
    return c_value;
}

ryu_state ryu_value_create_list(uint64_t num_elements, ryu_value** elements,
    ryu_value** out_value) {
    if (num_elements == 0) {
        return RyuError;
    }
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    std::vector<std::unique_ptr<Value>> children;

    auto first_element = static_cast<Value*>(elements[0]->_value);
    auto type = first_element->getDataType().copy();

    for (uint64_t i = 0; i < num_elements; ++i) {
        auto child = static_cast<Value*>(elements[i]->_value);
        if (child->getDataType() != type) {
            free(c_value);
            return RyuError;
        }
        // Copy the value to the list value to transfer ownership to the C++ side.
        children.push_back(child->copy());
    }
    auto list_type = LogicalType::LIST(first_element->getDataType().copy());
    c_value->_value = new Value(list_type.copy(), std::move(children));
    c_value->_is_owned_by_cpp = false;
    *out_value = c_value;
    return RyuSuccess;
}

ryu_state ryu_value_create_struct(uint64_t num_fields, const char** field_names,
    ryu_value** field_values, ryu_value** out_value) {
    if (num_fields == 0) {
        return RyuError;
    }
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    std::vector<std::unique_ptr<Value>> children;
    auto struct_fields = std::vector<StructField>{};
    for (uint64_t i = 0; i < num_fields; ++i) {
        auto field_name = std::string(field_names[i]);
        auto field_value = static_cast<Value*>(field_values[i]->_value);
        auto field_type = field_value->getDataType().copy();
        struct_fields.emplace_back(std::move(field_name), std::move(field_type));
        children.push_back(field_value->copy());
    }
    auto struct_type = LogicalType::STRUCT(std::move(struct_fields));
    c_value->_value = new Value(std::move(struct_type), std::move(children));
    c_value->_is_owned_by_cpp = false;
    *out_value = c_value;
    return RyuSuccess;
}

ryu_state ryu_value_create_map(uint64_t num_fields, ryu_value** keys, ryu_value** values,
    ryu_value** out_value) {
    if (num_fields == 0) {
        return RyuError;
    }
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    std::vector<std::unique_ptr<Value>> children;

    auto first_key = static_cast<Value*>(keys[0]->_value);
    auto first_value = static_cast<Value*>(values[0]->_value);
    auto key_type = first_key->getDataType().copy();
    auto value_type = first_value->getDataType().copy();

    for (uint64_t i = 0; i < num_fields; ++i) {
        auto key = static_cast<Value*>(keys[i]->_value);
        auto value = static_cast<Value*>(values[i]->_value);
        if (key->getDataType() != key_type || value->getDataType() != value_type) {
            free(c_value);
            return RyuError;
        }
        std::vector<StructField> struct_fields;
        struct_fields.emplace_back(InternalKeyword::MAP_KEY, key_type.copy());
        struct_fields.emplace_back(InternalKeyword::MAP_VALUE, value_type.copy());
        std::vector<std::unique_ptr<Value>> struct_values;
        struct_values.push_back(key->copy());
        struct_values.push_back(value->copy());
        auto struct_type = LogicalType::STRUCT(std::move(struct_fields));
        auto struct_value = new Value(std::move(struct_type), std::move(struct_values));
        children.push_back(std::unique_ptr<Value>(struct_value));
    }
    auto map_type = LogicalType::MAP(key_type.copy(), value_type.copy());
    c_value->_value = new Value(map_type.copy(), std::move(children));
    c_value->_is_owned_by_cpp = false;
    *out_value = c_value;
    return RyuSuccess;
}

ryu_value* ryu_value_clone(ryu_value* value) {
    auto* c_value = (ryu_value*)calloc(1, sizeof(ryu_value));
    c_value->_value = new Value(*static_cast<Value*>(value->_value));
    return c_value;
}

void ryu_value_copy(ryu_value* value, ryu_value* other) {
    static_cast<Value*>(value->_value)->copyValueFrom(*static_cast<Value*>(other->_value));
}

void ryu_value_destroy(ryu_value* value) {
    if (value == nullptr) {
        return;
    }
    if (!value->_is_owned_by_cpp) {
        if (value->_value != nullptr) {
            delete static_cast<Value*>(value->_value);
        }
        free(value);
    }
}

ryu_state ryu_value_get_list_size(ryu_value* value, uint64_t* out_result) {
    if (static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID() !=
        LogicalTypeID::LIST) {
        return RyuError;
    }
    *out_result = NestedVal::getChildrenSize(static_cast<Value*>(value->_value));
    return RyuSuccess;
}

ryu_state ryu_value_get_list_element(ryu_value* value, uint64_t index, ryu_value* out_value) {
    auto physical_type_id = static_cast<Value*>(value->_value)->getDataType().getPhysicalType();
    if (physical_type_id != PhysicalTypeID::ARRAY && physical_type_id != PhysicalTypeID::STRUCT &&
        physical_type_id != PhysicalTypeID::LIST) {
        return RyuError;
    }
    auto listValue = static_cast<Value*>(value->_value);
    if (index >= NestedVal::getChildrenSize(listValue)) {
        return RyuError;
    }
    try {
        auto val = NestedVal::getChildVal(listValue, index);
        out_value->_value = val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_struct_num_fields(ryu_value* value, uint64_t* out_result) {
    auto physical_type_id = static_cast<Value*>(value->_value)->getDataType().getPhysicalType();
    if (physical_type_id != PhysicalTypeID::STRUCT) {
        return RyuError;
    }
    auto val = static_cast<Value*>(value->_value);
    const auto& data_type = val->getDataType();
    try {
        *out_result = StructType::getNumFields(data_type);
        return RyuSuccess;
    } catch (Exception& e) {
        return RyuError;
    }
}

ryu_state ryu_value_get_struct_field_name(ryu_value* value, uint64_t index, char** out_result) {
    auto physical_type_id = static_cast<Value*>(value->_value)->getDataType().getPhysicalType();
    if (physical_type_id != PhysicalTypeID::STRUCT) {
        return RyuError;
    }
    auto val = static_cast<Value*>(value->_value);
    const auto& data_type = val->getDataType();
    if (index >= StructType::getNumFields(data_type)) {
        return RyuError;
    }
    std::string struct_field_name = StructType::getFields(data_type)[index].getName();
    if (struct_field_name.empty()) {
        return RyuError;
    }
    *out_result = convertToOwnedCString(struct_field_name);
    return RyuSuccess;
}

ryu_state ryu_value_get_struct_field_value(ryu_value* value, uint64_t index, ryu_value* out_value) {
    return ryu_value_get_list_element(value, index, out_value);
}

ryu_state ryu_value_get_map_size(ryu_value* value, uint64_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::MAP) {
        return RyuError;
    }
    auto listValue = static_cast<Value*>(value->_value);
    *out_result = NestedVal::getChildrenSize(listValue);
    return RyuSuccess;
}

ryu_state ryu_value_get_map_key(ryu_value* value, uint64_t index, ryu_value* out_key) {
    ryu_value map_entry;
    if (ryu_value_get_list_element(value, index, &map_entry) == RyuError) {
        return RyuError;
    }
    return ryu_value_get_struct_field_value(&map_entry, 0, out_key);
}

ryu_state ryu_value_get_map_value(ryu_value* value, uint64_t index, ryu_value* out_value) {
    ryu_value map_entry;
    if (ryu_value_get_list_element(value, index, &map_entry) == RyuError) {
        return RyuError;
    }
    return ryu_value_get_struct_field_value(&map_entry, 1, out_value);
}

ryu_state ryu_value_get_recursive_rel_node_list(ryu_value* value, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::RECURSIVE_REL) {
        return RyuError;
    }
    out_value->_is_owned_by_cpp = true;
    try {
        out_value->_value = RecursiveRelVal::getNodes(static_cast<Value*>(value->_value));
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_recursive_rel_rel_list(ryu_value* value, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::RECURSIVE_REL) {
        return RyuError;
    }
    out_value->_is_owned_by_cpp = true;
    try {
        out_value->_value = RecursiveRelVal::getRels(static_cast<Value*>(value->_value));
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

void ryu_value_get_data_type(ryu_value* value, ryu_logical_type* out_data_type) {
    out_data_type->_data_type =
        new LogicalType(static_cast<Value*>(value->_value)->getDataType().copy());
}

ryu_state ryu_value_get_bool(ryu_value* value, bool* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::BOOL) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<bool>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_int8(ryu_value* value, int8_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INT8) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<int8_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_int16(ryu_value* value, int16_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INT16) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<int16_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_int32(ryu_value* value, int32_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INT32) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<int32_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_int64(ryu_value* value, int64_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INT64) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<int64_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_uint8(ryu_value* value, uint8_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::UINT8) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<uint8_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_uint16(ryu_value* value, uint16_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::UINT16) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<uint16_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_uint32(ryu_value* value, uint32_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::UINT32) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<uint32_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_uint64(ryu_value* value, uint64_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::UINT64) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<uint64_t>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_int128(ryu_value* value, ryu_int128_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INT128) {
        return RyuError;
    }
    try {
        auto int128_val = static_cast<Value*>(value->_value)->getValue<int128_t>();
        out_result->low = int128_val.low;
        out_result->high = int128_val.high;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_int128_t_from_string(const char* str, ryu_int128_t* out_result) {
    int128_t int128_val = 0;
    try {
        ryu::function::CastString::operation(ku_string_t{str, strlen(str)}, int128_val);
        out_result->low = int128_val.low;
        out_result->high = int128_val.high;
    } catch (ConversionException& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_int128_t_to_string(ryu_int128_t int128_val, char** out_result) {
    int128_t c_int128 = 0;
    c_int128.low = int128_val.low;
    c_int128.high = int128_val.high;
    try {
        *out_result = convertToOwnedCString(TypeUtils::toString(c_int128));
    } catch (ConversionException& e) {
        return RyuError;
    }
    return RyuSuccess;
}
// TODO: bind all int128_t supported functions

ryu_state ryu_value_get_float(ryu_value* value, float* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::FLOAT) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<float>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_double(ryu_value* value, double* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::DOUBLE) {
        return RyuError;
    }
    try {
        *out_result = static_cast<Value*>(value->_value)->getValue<double>();
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_internal_id(ryu_value* value, ryu_internal_id_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INTERNAL_ID) {
        return RyuError;
    }
    try {
        auto id = static_cast<Value*>(value->_value)->getValue<internalID_t>();
        out_result->offset = id.offset;
        out_result->table_id = id.tableID;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_date(ryu_value* value, ryu_date_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::DATE) {
        return RyuError;
    }
    try {
        auto date_val = static_cast<Value*>(value->_value)->getValue<date_t>();
        out_result->days = date_val.days;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_timestamp(ryu_value* value, ryu_timestamp_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::TIMESTAMP) {
        return RyuError;
    }
    try {
        auto timestamp_val = static_cast<Value*>(value->_value)->getValue<timestamp_t>();
        out_result->value = timestamp_val.value;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_timestamp_ns(ryu_value* value, ryu_timestamp_ns_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::TIMESTAMP_NS) {
        return RyuError;
    }
    try {
        auto timestamp_val = static_cast<Value*>(value->_value)->getValue<timestamp_ns_t>();
        out_result->value = timestamp_val.value;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_timestamp_ms(ryu_value* value, ryu_timestamp_ms_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::TIMESTAMP_MS) {
        return RyuError;
    }
    try {
        auto timestamp_val = static_cast<Value*>(value->_value)->getValue<timestamp_ms_t>();
        out_result->value = timestamp_val.value;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_timestamp_sec(ryu_value* value, ryu_timestamp_sec_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::TIMESTAMP_SEC) {
        return RyuError;
    }
    try {
        auto timestamp_val = static_cast<Value*>(value->_value)->getValue<timestamp_sec_t>();
        out_result->value = timestamp_val.value;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_timestamp_tz(ryu_value* value, ryu_timestamp_tz_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::TIMESTAMP_TZ) {
        return RyuError;
    }
    try {
        auto timestamp_val = static_cast<Value*>(value->_value)->getValue<timestamp_tz_t>();
        out_result->value = timestamp_val.value;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_decimal_as_string(ryu_value* value, char** out_result) {
    auto decimal_val = static_cast<Value*>(value->_value);
    auto logical_type_id = decimal_val->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::DECIMAL) {
        return RyuError;
    }

    *out_result = convertToOwnedCString(decimal_val->toString());
    return RyuSuccess;
}

ryu_state ryu_value_get_interval(ryu_value* value, ryu_interval_t* out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::INTERVAL) {
        return RyuError;
    }
    try {
        auto interval_val = static_cast<Value*>(value->_value)->getValue<interval_t>();
        out_result->months = interval_val.months;
        out_result->days = interval_val.days;
        out_result->micros = interval_val.micros;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_string(ryu_value* value, char** out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::STRING) {
        return RyuError;
    }
    try {
        *out_result =
            convertToOwnedCString(static_cast<Value*>(value->_value)->getValue<std::string>());
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_blob(ryu_value* value, uint8_t** out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::BLOB) {
        return RyuError;
    }
    try {
        auto blob = static_cast<Value*>(value->_value)->getValue<std::string>();
        *out_result = (uint8_t*)convertToOwnedCString(blob);
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_value_get_uuid(ryu_value* value, char** out_result) {
    auto logical_type_id = static_cast<Value*>(value->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::UUID) {
        return RyuError;
    }
    try {
        *out_result =
            convertToOwnedCString(static_cast<Value*>(value->_value)->getValue<std::string>());
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

char* ryu_value_to_string(ryu_value* value) {
    return convertToOwnedCString(static_cast<Value*>(value->_value)->toString());
}

ryu_state ryu_node_val_get_id_val(ryu_value* node_val, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(node_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::NODE) {
        return RyuError;
    }
    try {
        auto id_val = NodeVal::getNodeIDVal(static_cast<Value*>(node_val->_value));
        out_value->_value = id_val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_node_val_get_label_val(ryu_value* node_val, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(node_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::NODE) {
        return RyuError;
    }
    try {
        auto label_val = NodeVal::getLabelVal(static_cast<Value*>(node_val->_value));
        out_value->_value = label_val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_node_val_get_property_size(ryu_value* node_val, uint64_t* out_result) {
    auto logical_type_id = static_cast<Value*>(node_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::NODE) {
        return RyuError;
    }
    try {
        *out_result = NodeVal::getNumProperties(static_cast<Value*>(node_val->_value));
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_node_val_get_property_name_at(ryu_value* node_val, uint64_t index,
    char** out_result) {
    auto logical_type_id = static_cast<Value*>(node_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::NODE) {
        return RyuError;
    }
    try {
        std::string property_name =
            NodeVal::getPropertyName(static_cast<Value*>(node_val->_value), index);
        if (property_name.empty()) {
            return RyuError;
        }
        *out_result = convertToOwnedCString(property_name);
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_node_val_get_property_value_at(ryu_value* node_val, uint64_t index,
    ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(node_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::NODE) {
        return RyuError;
    }
    try {
        auto value = NodeVal::getPropertyVal(static_cast<Value*>(node_val->_value), index);
        out_value->_value = value;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_node_val_to_string(ryu_value* node_val, char** out_result) {
    auto logical_type_id = static_cast<Value*>(node_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::NODE) {
        return RyuError;
    }
    try {
        *out_result =
            convertToOwnedCString(NodeVal::toString(static_cast<Value*>(node_val->_value)));
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_get_id_val(ryu_value* rel_val, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        auto id_val = RelVal::getIDVal(static_cast<Value*>(rel_val->_value));
        out_value->_value = id_val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_get_src_id_val(ryu_value* rel_val, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        auto src_id_val = RelVal::getSrcNodeIDVal(static_cast<Value*>(rel_val->_value));
        out_value->_value = src_id_val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_get_dst_id_val(ryu_value* rel_val, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        auto dst_id_val = RelVal::getDstNodeIDVal(static_cast<Value*>(rel_val->_value));
        out_value->_value = dst_id_val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_get_label_val(ryu_value* rel_val, ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        auto label_val = RelVal::getLabelVal(static_cast<Value*>(rel_val->_value));
        out_value->_value = label_val;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_get_property_size(ryu_value* rel_val, uint64_t* out_result) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        *out_result = RelVal::getNumProperties(static_cast<Value*>(rel_val->_value));
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}
ryu_state ryu_rel_val_get_property_name_at(ryu_value* rel_val, uint64_t index, char** out_result) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        std::string property_name =
            RelVal::getPropertyName(static_cast<Value*>(rel_val->_value), index);
        if (property_name.empty()) {
            return RyuError;
        }
        *out_result = convertToOwnedCString(property_name);
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_get_property_value_at(ryu_value* rel_val, uint64_t index,
    ryu_value* out_value) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        auto value = RelVal::getPropertyVal(static_cast<Value*>(rel_val->_value), index);
        out_value->_value = value;
        out_value->_is_owned_by_cpp = true;
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

ryu_state ryu_rel_val_to_string(ryu_value* rel_val, char** out_result) {
    auto logical_type_id = static_cast<Value*>(rel_val->_value)->getDataType().getLogicalTypeID();
    if (logical_type_id != LogicalTypeID::REL) {
        return RyuError;
    }
    try {
        *out_result = convertToOwnedCString(RelVal::toString(static_cast<Value*>(rel_val->_value)));
    } catch (Exception& e) {
        return RyuError;
    }
    return RyuSuccess;
}

void ryu_destroy_string(char* str) {
    free(str);
}

void ryu_destroy_blob(uint8_t* blob) {
    free(blob);
}

ryu_state ryu_timestamp_ns_to_tm(ryu_timestamp_ns_t timestamp, struct tm* out_result) {
    time_t time = timestamp.value / 1000000000;
#ifdef _WIN32
    if (convertTimeToTm(time, out_result) != 0) {
        return RyuError;
    }
#else
    if (gmtime_r(&time, out_result) == nullptr) {
        return RyuError;
    }
#endif
    return RyuSuccess;
}

ryu_state ryu_timestamp_ms_to_tm(ryu_timestamp_ms_t timestamp, struct tm* out_result) {
    time_t time = timestamp.value / 1000;
#ifdef _WIN32
    if (convertTimeToTm(time, out_result) != 0) {
        return RyuError;
    }
#else
    if (gmtime_r(&time, out_result) == nullptr) {
        return RyuError;
    }
#endif
    return RyuSuccess;
}

ryu_state ryu_timestamp_sec_to_tm(ryu_timestamp_sec_t timestamp, struct tm* out_result) {
    time_t time = timestamp.value;
#ifdef _WIN32
    if (convertTimeToTm(time, out_result) != 0) {
        return RyuError;
    }
#else
    if (gmtime_r(&time, out_result) == nullptr) {
        return RyuError;
    }
#endif
    return RyuSuccess;
}

ryu_state ryu_timestamp_tz_to_tm(ryu_timestamp_tz_t timestamp, struct tm* out_result) {
    time_t time = timestamp.value / 1000000;
#ifdef _WIN32
    if (convertTimeToTm(time, out_result) != 0) {
        return RyuError;
    }
#else
    if (gmtime_r(&time, out_result) == nullptr) {
        return RyuError;
    }
#endif
    return RyuSuccess;
}

ryu_state ryu_timestamp_to_tm(ryu_timestamp_t timestamp, struct tm* out_result) {
    time_t time = timestamp.value / 1000000;
#ifdef _WIN32
    if (convertTimeToTm(time, out_result) != 0) {
        return RyuError;
    }
#else
    if (gmtime_r(&time, out_result) == nullptr) {
        return RyuError;
    }
#endif
    return RyuSuccess;
}

ryu_state ryu_timestamp_ns_from_tm(struct tm tm, ryu_timestamp_ns_t* out_result) {
#ifdef _WIN32
    int64_t time = convertTmToTime(tm);
#else
    int64_t time = timegm(&tm);
#endif
    if (time == -1) {
        return RyuError;
    }
    out_result->value = time * 1000000000;
    return RyuSuccess;
}

ryu_state ryu_timestamp_ms_from_tm(struct tm tm, ryu_timestamp_ms_t* out_result) {
#ifdef _WIN32
    int64_t time = convertTmToTime(tm);
#else
    int64_t time = timegm(&tm);
#endif
    if (time == -1) {
        return RyuError;
    }
    out_result->value = time * 1000;
    return RyuSuccess;
}

ryu_state ryu_timestamp_sec_from_tm(struct tm tm, ryu_timestamp_sec_t* out_result) {
#ifdef _WIN32
    int64_t time = convertTmToTime(tm);
#else
    int64_t time = timegm(&tm);
#endif
    if (time == -1) {
        return RyuError;
    }
    out_result->value = time;
    return RyuSuccess;
}

ryu_state ryu_timestamp_tz_from_tm(struct tm tm, ryu_timestamp_tz_t* out_result) {
#ifdef _WIN32
    int64_t time = convertTmToTime(tm);
#else
    int64_t time = timegm(&tm);
#endif
    if (time == -1) {
        return RyuError;
    }
    out_result->value = time * 1000000;
    return RyuSuccess;
}

ryu_state ryu_timestamp_from_tm(struct tm tm, ryu_timestamp_t* out_result) {
#ifdef _WIN32
    int64_t time = convertTmToTime(tm);
#else
    int64_t time = timegm(&tm);
#endif
    if (time == -1) {
        return RyuError;
    }
    out_result->value = time * 1000000;
    return RyuSuccess;
}

ryu_state ryu_date_to_tm(ryu_date_t date, struct tm* out_result) {
    time_t time = date.days * 86400;
#ifdef _WIN32
    if (convertTimeToTm(time, out_result) != 0) {
        return RyuError;
    }
#else
    if (gmtime_r(&time, out_result) == nullptr) {
        return RyuError;
    }
#endif
    out_result->tm_hour = 0;
    out_result->tm_min = 0;
    out_result->tm_sec = 0;
    return RyuSuccess;
}

ryu_state ryu_date_from_tm(struct tm tm, ryu_date_t* out_result) {
#ifdef _WIN32
    int64_t time = convertTmToTime(tm);
#else
    int64_t time = timegm(&tm);
#endif
    if (time == -1) {
        return RyuError;
    }
    out_result->days = time / 86400;
    return RyuSuccess;
}

ryu_state ryu_date_to_string(ryu_date_t date, char** out_result) {
    tm tm{};
    if (ryu_date_to_tm(date, &tm) != RyuSuccess) {
        return RyuError;
    }
    char buffer[80];
    if (strftime(buffer, 80, "%Y-%m-%d", &tm) == 0) {
        return RyuError;
    }
    *out_result = convertToOwnedCString(buffer);
    return RyuSuccess;
}

ryu_state ryu_date_from_string(const char* str, ryu_date_t* out_result) {
    try {
        date_t date = Date::fromCString(str, strlen(str));
        out_result->days = date.days;
    } catch (ConversionException& e) {
        return RyuError;
    }
    return RyuSuccess;
}

void ryu_interval_to_difftime(ryu_interval_t interval, double* out_result) {
    auto micros = interval.micros + interval.months * Interval::MICROS_PER_MONTH +
                  interval.days * Interval::MICROS_PER_DAY;
    double seconds = micros / 1000000.0;
    *out_result = seconds;
}

void ryu_interval_from_difftime(double difftime, ryu_interval_t* out_result) {
    int64_t total_micros = static_cast<int64_t>(difftime * 1000000);
    out_result->months = total_micros / Interval::MICROS_PER_MONTH;
    total_micros -= out_result->months * Interval::MICROS_PER_MONTH;
    out_result->days = total_micros / Interval::MICROS_PER_DAY;
    total_micros -= out_result->days * Interval::MICROS_PER_DAY;
    out_result->micros = total_micros;
}
