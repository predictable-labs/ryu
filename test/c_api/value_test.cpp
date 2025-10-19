#include "c_api_test/c_api_test.h"

using namespace ryu::main;
using namespace ryu::common;
using namespace ryu::testing;

class CApiValueTest : public CApiTest {
public:
    std::string getInputDir() override { return TestHelper::appendRyuRootPath("dataset/tinysnb/"); }
};

TEST(CApiValueTestEmptyDB, CreateNull) {
    ryu_value* value = ryu_value_create_null();
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::ANY);
    ASSERT_EQ(cppValue->isNull(), true);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateNullWithDatatype) {
    ryu_logical_type type;
    ryu_data_type_create(ryu_data_type_id::RYU_INT64, nullptr, 0, &type);
    ryu_value* value = ryu_value_create_null_with_data_type(&type);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    ryu_data_type_destroy(&type);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT64);
    ASSERT_EQ(cppValue->isNull(), true);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, IsNull) {
    ryu_value* value = ryu_value_create_int64(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(value));
    ryu_value_destroy(value);
    value = ryu_value_create_null();
    ASSERT_TRUE(ryu_value_is_null(value));
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, SetNull) {
    ryu_value* value = ryu_value_create_int64(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(value));
    ryu_value_set_null(value, true);
    ASSERT_TRUE(ryu_value_is_null(value));
    ryu_value_set_null(value, false);
    ASSERT_FALSE(ryu_value_is_null(value));
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateDefault) {
    ryu_logical_type type;
    ryu_data_type_create(ryu_data_type_id::RYU_INT64, nullptr, 0, &type);
    ryu_value* value = ryu_value_create_default(&type);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    ryu_data_type_destroy(&type);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_FALSE(ryu_value_is_null(value));
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT64);
    ASSERT_EQ(cppValue->getValue<int64_t>(), 0);
    ryu_value_destroy(value);

    ryu_data_type_create(ryu_data_type_id::RYU_STRING, nullptr, 0, &type);
    value = ryu_value_create_default(&type);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    ryu_data_type_destroy(&type);
    cppValue = static_cast<Value*>(value->_value);
    ASSERT_FALSE(ryu_value_is_null(value));
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::STRING);
    ASSERT_EQ(cppValue->getValue<std::string>(), "");
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateBool) {
    ryu_value* value = ryu_value_create_bool(true);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::BOOL);
    ASSERT_EQ(cppValue->getValue<bool>(), true);
    ryu_value_destroy(value);

    value = ryu_value_create_bool(false);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::BOOL);
    ASSERT_EQ(cppValue->getValue<bool>(), false);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateInt8) {
    ryu_value* value = ryu_value_create_int8(12);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT8);
    ASSERT_EQ(cppValue->getValue<int8_t>(), 12);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateInt16) {
    ryu_value* value = ryu_value_create_int16(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT16);
    ASSERT_EQ(cppValue->getValue<int16_t>(), 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateInt32) {
    ryu_value* value = ryu_value_create_int32(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT32);
    ASSERT_EQ(cppValue->getValue<int32_t>(), 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateInt64) {
    ryu_value* value = ryu_value_create_int64(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT64);
    ASSERT_EQ(cppValue->getValue<int64_t>(), 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateUInt8) {
    ryu_value* value = ryu_value_create_uint8(12);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::UINT8);
    ASSERT_EQ(cppValue->getValue<uint8_t>(), 12);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateUInt16) {
    ryu_value* value = ryu_value_create_uint16(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::UINT16);
    ASSERT_EQ(cppValue->getValue<uint16_t>(), 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateUInt32) {
    ryu_value* value = ryu_value_create_uint32(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::UINT32);
    ASSERT_EQ(cppValue->getValue<uint32_t>(), 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateUInt64) {
    ryu_value* value = ryu_value_create_uint64(123);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::UINT64);
    ASSERT_EQ(cppValue->getValue<uint64_t>(), 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateINT128) {
    ryu_value* value = ryu_value_create_int128(ryu_int128_t{211111111, 100000000});
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INT128);
    auto cppTimeStamp = cppValue->getValue<int128_t>();
    ASSERT_EQ(cppTimeStamp.high, 100000000);
    ASSERT_EQ(cppTimeStamp.low, 211111111);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateFloat) {
    ryu_value* value = ryu_value_create_float(123.456);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::FLOAT);
    ASSERT_FLOAT_EQ(cppValue->getValue<float>(), 123.456);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateDouble) {
    ryu_value* value = ryu_value_create_double(123.456);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::DOUBLE);
    ASSERT_DOUBLE_EQ(cppValue->getValue<double>(), 123.456);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateInternalID) {
    auto internalID = ryu_internal_id_t{1, 123};
    ryu_value* value = ryu_value_create_internal_id(internalID);
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INTERNAL_ID);
    auto internalIDCpp = cppValue->getValue<internalID_t>();
    ASSERT_EQ(internalIDCpp.tableID, 1);
    ASSERT_EQ(internalIDCpp.offset, 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateDate) {
    ryu_value* value = ryu_value_create_date(ryu_date_t{123});
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::DATE);
    auto cppDate = cppValue->getValue<date_t>();
    ASSERT_EQ(cppDate.days, 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateTimeStamp) {
    ryu_value* value = ryu_value_create_timestamp(ryu_timestamp_t{123});
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::TIMESTAMP);
    auto cppTimeStamp = cppValue->getValue<timestamp_t>();
    ASSERT_EQ(cppTimeStamp.value, 123);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateTimeStampNonStandard) {
    ryu_value* value_ns = ryu_value_create_timestamp_ns(ryu_timestamp_ns_t{12345});
    ryu_value* value_ms = ryu_value_create_timestamp_ms(ryu_timestamp_ms_t{123456});
    ryu_value* value_sec = ryu_value_create_timestamp_sec(ryu_timestamp_sec_t{1234567});
    ryu_value* value_tz = ryu_value_create_timestamp_tz(ryu_timestamp_tz_t{12345678});

    ASSERT_FALSE(value_ns->_is_owned_by_cpp);
    ASSERT_FALSE(value_ms->_is_owned_by_cpp);
    ASSERT_FALSE(value_sec->_is_owned_by_cpp);
    ASSERT_FALSE(value_tz->_is_owned_by_cpp);
    auto cppValue_ns = static_cast<Value*>(value_ns->_value);
    auto cppValue_ms = static_cast<Value*>(value_ms->_value);
    auto cppValue_sec = static_cast<Value*>(value_sec->_value);
    auto cppValue_tz = static_cast<Value*>(value_tz->_value);
    ASSERT_EQ(cppValue_ns->getDataType().getLogicalTypeID(), LogicalTypeID::TIMESTAMP_NS);
    ASSERT_EQ(cppValue_ms->getDataType().getLogicalTypeID(), LogicalTypeID::TIMESTAMP_MS);
    ASSERT_EQ(cppValue_sec->getDataType().getLogicalTypeID(), LogicalTypeID::TIMESTAMP_SEC);
    ASSERT_EQ(cppValue_tz->getDataType().getLogicalTypeID(), LogicalTypeID::TIMESTAMP_TZ);

    auto cppTimeStamp_ns = cppValue_ns->getValue<timestamp_ns_t>();
    auto cppTimeStamp_ms = cppValue_ms->getValue<timestamp_ms_t>();
    auto cppTimeStamp_sec = cppValue_sec->getValue<timestamp_sec_t>();
    auto cppTimeStamp_tz = cppValue_tz->getValue<timestamp_tz_t>();
    ASSERT_EQ(cppTimeStamp_ns.value, 12345);
    ASSERT_EQ(cppTimeStamp_ms.value, 123456);
    ASSERT_EQ(cppTimeStamp_sec.value, 1234567);
    ASSERT_EQ(cppTimeStamp_tz.value, 12345678);
    ryu_value_destroy(value_ns);
    ryu_value_destroy(value_ms);
    ryu_value_destroy(value_sec);
    ryu_value_destroy(value_tz);
}

TEST(CApiValueTestEmptyDB, CreateInterval) {
    ryu_value* value = ryu_value_create_interval(ryu_interval_t{12, 3, 300});
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::INTERVAL);
    auto cppTimeStamp = cppValue->getValue<interval_t>();
    ASSERT_EQ(cppTimeStamp.months, 12);
    ASSERT_EQ(cppTimeStamp.days, 3);
    ASSERT_EQ(cppTimeStamp.micros, 300);
    ryu_value_destroy(value);
}

TEST(CApiValueTestEmptyDB, CreateString) {
    ryu_value* value = ryu_value_create_string((char*)"abcdefg");
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::STRING);
    ASSERT_EQ(cppValue->getValue<std::string>(), "abcdefg");
    ryu_value_destroy(value);
}

TEST_F(CApiValueTest, CreateList) {
    auto connection = getConnection();
    ryu_value* value1 = ryu_value_create_int64(123);
    ryu_value* value2 = ryu_value_create_int64(456);
    ryu_value* value3 = ryu_value_create_int64(789);
    ryu_value* value4 = ryu_value_create_int64(101112);
    ryu_value* value5 = ryu_value_create_int64(131415);
    ryu_value* elements[] = {value1, value2, value3, value4, value5};
    ryu_value* value = nullptr;
    ryu_state state = ryu_value_create_list(5, elements, &value);
    ASSERT_EQ(state, RyuSuccess);
    // Destroy the original values, the list should still be valid
    for (int i = 0; i < 5; ++i) {
        ryu_value_destroy(elements[i]);
    }
    ASSERT_FALSE(value->_is_owned_by_cpp);
    ryu_prepared_statement stmt;
    state = ryu_connection_prepare(connection, (char*)"RETURN $1", &stmt);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_prepared_statement_bind_value(&stmt, "1", value);
    ASSERT_EQ(state, RyuSuccess);
    ryu_query_result result;
    state = ryu_connection_execute(connection, &stmt, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    ryu_flat_tuple flatTuple;
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value outValue;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &outValue), RyuSuccess);
    ASSERT_TRUE(outValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&outValue));
    uint64_t size;
    ASSERT_EQ(ryu_value_get_list_size(&outValue, &size), RyuSuccess);
    ASSERT_EQ(size, 5);
    ryu_value listElement;
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 0, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    int64_t int64Result;
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 123);
    ryu_value_destroy(&listElement);
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 1, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 456);
    ryu_value_destroy(&listElement);
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 2, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 789);
    ryu_value_destroy(&listElement);
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 3, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 101112);
    ryu_value_destroy(&listElement);
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 4, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 131415);
    ryu_value_destroy(&listElement);
    ryu_value_destroy(&outValue);
    ryu_value_destroy(value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&stmt);
}

TEST(CApiValueTestEmptyDB, CreateListDifferentTypes) {
    ryu_value* value1 = ryu_value_create_int64(123);
    ryu_value* value2 = ryu_value_create_string((char*)"abcdefg");
    ryu_value* elements[] = {value1, value2};
    ryu_value* value = nullptr;
    ryu_state state = ryu_value_create_list(2, elements, &value);
    ASSERT_EQ(state, RyuError);
    ryu_value_destroy(value1);
    ryu_value_destroy(value2);
}

TEST(CApiValueTestEmptyDB, CreateListEmpty) {
    ryu_value* elements[] = {nullptr}; // Must be non-empty
    ryu_value* value = nullptr;
    ryu_state state = ryu_value_create_list(0, elements, &value);
    ASSERT_EQ(state, RyuError);
}

TEST_F(CApiValueTest, CreateListNested) {
    auto connection = getConnection();
    ryu_value* value1 = ryu_value_create_int64(123);
    ryu_value* value2 = ryu_value_create_int64(456);
    ryu_value* value3 = ryu_value_create_int64(789);
    ryu_value* value4 = ryu_value_create_int64(101112);
    ryu_value* value5 = ryu_value_create_int64(131415);
    ryu_value* elements1[] = {value1, value2, value3};
    ryu_value* elements2[] = {value4, value5};
    ryu_value* list1 = nullptr;
    ryu_value* list2 = nullptr;
    ryu_value_create_list(3, elements1, &list1);
    ASSERT_FALSE(list1->_is_owned_by_cpp);
    ryu_value_create_list(2, elements2, &list2);
    ASSERT_FALSE(list2->_is_owned_by_cpp);
    ryu_value* elements[] = {list1, list2};
    ryu_value* nestedList = nullptr;
    ryu_state state = ryu_value_create_list(2, elements, &nestedList);
    ASSERT_EQ(state, RyuSuccess);
    // Destroy the original values, the list should still be valid
    for (int i = 0; i < 3; ++i) {
        ryu_value_destroy(elements1[i]);
    }
    for (int i = 0; i < 2; ++i) {
        ryu_value_destroy(elements2[i]);
    }
    ryu_value_destroy(list1);
    ryu_value_destroy(list2);
    ASSERT_FALSE(nestedList->_is_owned_by_cpp);
    ryu_prepared_statement stmt;
    state = ryu_connection_prepare(connection, (char*)"RETURN $1", &stmt);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_prepared_statement_bind_value(&stmt, "1", nestedList);
    ASSERT_EQ(state, RyuSuccess);
    ryu_query_result result;
    state = ryu_connection_execute(connection, &stmt, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    ryu_flat_tuple flatTuple;
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value outValue;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &outValue), RyuSuccess);
    ASSERT_TRUE(outValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&outValue));
    uint64_t size;
    ASSERT_EQ(ryu_value_get_list_size(&outValue, &size), RyuSuccess);
    ASSERT_EQ(size, 2);
    ryu_value listElement;
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 0, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&listElement));
    ASSERT_EQ(ryu_value_get_list_size(&listElement, &size), RyuSuccess);
    ASSERT_EQ(size, 3);
    ryu_value innerListElement;
    ASSERT_EQ(ryu_value_get_list_element(&listElement, 0, &innerListElement), RyuSuccess);
    ASSERT_TRUE(innerListElement._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&innerListElement));
    int64_t int64Result;
    ASSERT_EQ(ryu_value_get_int64(&innerListElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 123);
    ryu_value_destroy(&innerListElement);
    ASSERT_EQ(ryu_value_get_list_element(&listElement, 1, &innerListElement), RyuSuccess);
    ASSERT_TRUE(innerListElement._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&innerListElement));
    ASSERT_EQ(ryu_value_get_int64(&innerListElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 456);
    ryu_value_destroy(&innerListElement);
    ASSERT_EQ(ryu_value_get_list_element(&listElement, 2, &innerListElement), RyuSuccess);
    ASSERT_TRUE(innerListElement._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&innerListElement));
    ASSERT_EQ(ryu_value_get_int64(&innerListElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 789);
    ryu_value_destroy(&innerListElement);
    ryu_value_destroy(&listElement);
    ASSERT_EQ(ryu_value_get_list_element(&outValue, 1, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&listElement));
    ASSERT_EQ(ryu_value_get_list_size(&listElement, &size), RyuSuccess);
    ASSERT_EQ(size, 2);
    ryu_value_destroy(&listElement);
    ryu_value_destroy(&outValue);
    ryu_value_destroy(nestedList);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&stmt);
}

TEST_F(CApiValueTest, CreateStruct) {
    auto connection = getConnection();
    ryu_value* value1 = ryu_value_create_int16(32);
    ryu_value* value2 = ryu_value_create_string((char*)"Wong");
    ryu_value* value3 = ryu_value_create_string((char*)"Kelley");
    ryu_value* value4 = ryu_value_create_int64(123456);
    ryu_value* value5 = ryu_value_create_string((char*)"CEO");
    ryu_value* value6 = ryu_value_create_bool(true);
    ryu_value* employmentElements[] = {value5, value6};
    const char* employmentFieldNames[] = {(char*)"title", (char*)"is_current"};
    ryu_value* employment = nullptr;
    ryu_state state =
        ryu_value_create_struct(2, employmentFieldNames, employmentElements, &employment);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_FALSE(employment->_is_owned_by_cpp);
    ryu_value_destroy(value5);
    ryu_value_destroy(value6);
    ryu_value* personElements[] = {value1, value2, value3, value4, employment};
    const char* personFieldNames[] = {(char*)"age", (char*)"first_name", (char*)"last_name",
        (char*)"id", (char*)"employment"};
    ryu_value* person = nullptr;
    state = ryu_value_create_struct(5, personFieldNames, personElements, &person);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value_destroy(value1);
    ryu_value_destroy(value2);
    ryu_value_destroy(value3);
    ryu_value_destroy(value4);
    ryu_value_destroy(employment);
    ASSERT_FALSE(person->_is_owned_by_cpp);
    ryu_prepared_statement stmt;
    state = ryu_connection_prepare(connection, (char*)"RETURN $1", &stmt);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_prepared_statement_bind_value(&stmt, "1", person);
    ASSERT_EQ(state, RyuSuccess);
    ryu_query_result result;
    state = ryu_connection_execute(connection, &stmt, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    ryu_flat_tuple flatTuple;
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value outValue;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &outValue), RyuSuccess);
    ASSERT_TRUE(outValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&outValue));
    uint64_t size;
    state = ryu_value_get_struct_num_fields(&outValue, &size);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(size, 5);
    char* structFieldName;
    ryu_value structFieldValue;
    state = ryu_value_get_struct_field_name(&outValue, 0, &structFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(structFieldName, "age");
    state = ryu_value_get_struct_field_value(&outValue, 0, &structFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(structFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&structFieldValue));
    int16_t int16Result;
    state = ryu_value_get_int16(&structFieldValue, &int16Result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(int16Result, 32);
    ryu_value_destroy(&structFieldValue);
    ryu_destroy_string(structFieldName);
    state = ryu_value_get_struct_field_name(&outValue, 1, &structFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(structFieldName, "first_name");
    state = ryu_value_get_struct_field_value(&outValue, 1, &structFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(structFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&structFieldValue));
    char* stringResult;
    state = ryu_value_get_string(&structFieldValue, &stringResult);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(stringResult, "Wong");
    ryu_value_destroy(&structFieldValue);
    ryu_destroy_string(structFieldName);
    ryu_destroy_string(stringResult);
    state = ryu_value_get_struct_field_name(&outValue, 2, &structFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(structFieldName, "last_name");
    state = ryu_value_get_struct_field_value(&outValue, 2, &structFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(structFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&structFieldValue));
    state = ryu_value_get_string(&structFieldValue, &stringResult);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(stringResult, "Kelley");
    ryu_value_destroy(&structFieldValue);
    ryu_destroy_string(structFieldName);
    ryu_destroy_string(stringResult);
    state = ryu_value_get_struct_field_name(&outValue, 3, &structFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(structFieldName, "id");
    state = ryu_value_get_struct_field_value(&outValue, 3, &structFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(structFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&structFieldValue));
    int64_t int64Result;
    state = ryu_value_get_int64(&structFieldValue, &int64Result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(int64Result, 123456);
    ryu_value_destroy(&structFieldValue);
    ryu_destroy_string(structFieldName);
    state = ryu_value_get_struct_field_name(&outValue, 4, &structFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(structFieldName, "employment");
    state = ryu_value_get_struct_field_value(&outValue, 4, &structFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(structFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&structFieldValue));
    state = ryu_value_get_struct_num_fields(&structFieldValue, &size);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(size, 2);
    char* employmentFieldName;
    ryu_value employmentFieldValue;
    state = ryu_value_get_struct_field_name(&structFieldValue, 0, &employmentFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(employmentFieldName, "title");
    state = ryu_value_get_struct_field_value(&structFieldValue, 0, &employmentFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(employmentFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&employmentFieldValue));
    state = ryu_value_get_string(&employmentFieldValue, &stringResult);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(stringResult, "CEO");
    ryu_value_destroy(&employmentFieldValue);
    ryu_destroy_string(employmentFieldName);
    ryu_destroy_string(stringResult);
    state = ryu_value_get_struct_field_name(&structFieldValue, 1, &employmentFieldName);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_STREQ(employmentFieldName, "is_current");
    state = ryu_value_get_struct_field_value(&structFieldValue, 1, &employmentFieldValue);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(employmentFieldValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&employmentFieldValue));
    bool boolResult;
    state = ryu_value_get_bool(&employmentFieldValue, &boolResult);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(boolResult, true);
    ryu_value_destroy(&employmentFieldValue);
    ryu_destroy_string(employmentFieldName);
    ryu_value_destroy(&structFieldValue);
    ryu_destroy_string(structFieldName);
    ryu_value_destroy(&outValue);
    ryu_value_destroy(person);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&stmt);
}

TEST(CApiValueTestEmptyDB, CreateStructEmpty) {
    const char* fieldNames[] = {(char*)"name"}; // Must be non-empty
    ryu_value* values[] = {nullptr};            // Must be non-empty
    ryu_value* value = nullptr;
    ryu_state state = ryu_value_create_struct(0, fieldNames, values, &value);
    ASSERT_EQ(state, RyuError);
}

TEST_F(CApiValueTest, CreateMap) {
    auto connection = getConnection();
    ryu_value* key1 = ryu_value_create_int64(1);
    ryu_value* value1 = ryu_value_create_string((char*)"one");
    ryu_value* key2 = ryu_value_create_int64(2);
    ryu_value* value2 = ryu_value_create_string((char*)"two");
    ryu_value* key3 = ryu_value_create_int64(3);
    ryu_value* value3 = ryu_value_create_string((char*)"three");
    ryu_value* keys[] = {key1, key2, key3};
    ryu_value* values[] = {value1, value2, value3};
    ryu_value* map = nullptr;
    ryu_state state = ryu_value_create_map(3, keys, values, &map);
    ASSERT_EQ(state, RyuSuccess);
    // Destroy the original values, the map should still be valid
    for (int i = 0; i < 3; ++i) {
        ryu_value_destroy(keys[i]);
        ryu_value_destroy(values[i]);
    }
    ASSERT_FALSE(map->_is_owned_by_cpp);
    ryu_prepared_statement stmt;
    state = ryu_connection_prepare(connection, (char*)"RETURN $1", &stmt);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_prepared_statement_bind_value(&stmt, "1", map);
    ASSERT_EQ(state, RyuSuccess);
    ryu_query_result result;
    state = ryu_connection_execute(connection, &stmt, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    ryu_flat_tuple flatTuple;
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value outValue;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &outValue), RyuSuccess);
    ASSERT_TRUE(outValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&outValue));
    uint64_t size;
    ASSERT_EQ(ryu_value_get_map_size(&outValue, &size), RyuSuccess);
    ASSERT_EQ(size, 3);
    ryu_value mapValue;
    ASSERT_EQ(ryu_value_get_map_value(&outValue, 0, &mapValue), RyuSuccess);
    ASSERT_TRUE(mapValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&mapValue));
    char* stringResult;
    ASSERT_EQ(ryu_value_get_string(&mapValue, &stringResult), RyuSuccess);
    ASSERT_STREQ(stringResult, "one");
    ryu_value_destroy(&mapValue);
    ryu_destroy_string(stringResult);
    ASSERT_EQ(ryu_value_get_map_value(&outValue, 1, &mapValue), RyuSuccess);
    ASSERT_TRUE(mapValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&mapValue));
    ASSERT_EQ(ryu_value_get_string(&mapValue, &stringResult), RyuSuccess);
    ASSERT_STREQ(stringResult, "two");
    ryu_value_destroy(&mapValue);
    ryu_destroy_string(stringResult);
    ASSERT_EQ(ryu_value_get_map_value(&outValue, 2, &mapValue), RyuSuccess);
    ASSERT_TRUE(mapValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&mapValue));
    ASSERT_EQ(ryu_value_get_string(&mapValue, &stringResult), RyuSuccess);
    ASSERT_STREQ(stringResult, "three");
    ryu_value_destroy(&mapValue);
    ryu_destroy_string(stringResult);
    ASSERT_EQ(ryu_value_get_map_key(&outValue, 0, &mapValue), RyuSuccess);
    ASSERT_TRUE(mapValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&mapValue));
    int64_t int64Result;
    ASSERT_EQ(ryu_value_get_int64(&mapValue, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 1);
    ryu_value_destroy(&mapValue);
    ASSERT_EQ(ryu_value_get_map_key(&outValue, 1, &mapValue), RyuSuccess);
    ASSERT_TRUE(mapValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&mapValue));
    ASSERT_EQ(ryu_value_get_int64(&mapValue, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 2);
    ryu_value_destroy(&mapValue);
    ASSERT_EQ(ryu_value_get_map_key(&outValue, 2, &mapValue), RyuSuccess);
    ASSERT_TRUE(mapValue._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&mapValue));
    ASSERT_EQ(ryu_value_get_int64(&mapValue, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 3);
    ryu_value_destroy(&mapValue);
    ryu_value_destroy(&outValue);
    ryu_value_destroy(map);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&stmt);
}

TEST(CApiValueTestEmptyDB, CreateMapEmpty) {
    ryu_value* keys[] = {nullptr};   // Must be non-empty
    ryu_value* values[] = {nullptr}; // Must be non-empty
    ryu_value* map = nullptr;
    ryu_state state = ryu_value_create_map(0, keys, values, &map);
    ASSERT_EQ(state, RyuError);
}

TEST(CApiValueTestEmptyDB, Clone) {
    ryu_value* value = ryu_value_create_string((char*)"abcdefg");
    ASSERT_FALSE(value->_is_owned_by_cpp);
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::STRING);
    ASSERT_EQ(cppValue->getValue<std::string>(), "abcdefg");

    ryu_value* clone = ryu_value_clone(value);
    ryu_value_destroy(value);

    ASSERT_FALSE(clone->_is_owned_by_cpp);
    auto cppClone = static_cast<Value*>(clone->_value);
    ASSERT_EQ(cppClone->getDataType().getLogicalTypeID(), LogicalTypeID::STRING);
    ASSERT_EQ(cppClone->getValue<std::string>(), "abcdefg");
    ryu_value_destroy(clone);
}

TEST(CApiValueTestEmptyDB, Copy) {
    ryu_value* value = ryu_value_create_string((char*)"abc");

    ryu_value* value2 = ryu_value_create_string((char*)"abcdefg");
    ryu_value_copy(value, value2);
    ryu_value_destroy(value2);

    ASSERT_FALSE(ryu_value_is_null(value));
    auto cppValue = static_cast<Value*>(value->_value);
    ASSERT_EQ(cppValue->getDataType().getLogicalTypeID(), LogicalTypeID::STRING);
    ASSERT_EQ(cppValue->getValue<std::string>(), "abcdefg");
    ryu_value_destroy(value);
}

TEST_F(CApiValueTest, GetListSize) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.workedHours ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint64_t size;
    ASSERT_EQ(ryu_value_get_list_size(&value, &size), RyuSuccess);
    ASSERT_EQ(size, 2);

    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_list_size(badValue, &size), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetListElement) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.workedHours ORDER BY a.ID", &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint64_t size;
    ASSERT_EQ(ryu_value_get_list_size(&value, &size), RyuSuccess);
    ASSERT_EQ(size, 2);

    ryu_value listElement;
    ASSERT_EQ(ryu_value_get_list_element(&value, 0, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    int64_t int64Result;
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 10);

    ASSERT_EQ(ryu_value_get_list_element(&value, 1, &listElement), RyuSuccess);
    ASSERT_TRUE(listElement._is_owned_by_cpp);
    ASSERT_EQ(ryu_value_get_int64(&listElement, &int64Result), RyuSuccess);
    ASSERT_EQ(int64Result, 5);
    ryu_value_destroy(&listElement);

    ASSERT_EQ(ryu_value_get_list_element(&value, 222, &listElement), RyuError);

    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiValueTest, GetStructNumFields) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) WHERE m.name=\"Roma\" RETURN m.description", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ryu_flat_tuple_get_value(&flatTuple, 0, &value);
    uint64_t numFields;
    ASSERT_EQ(ryu_value_get_struct_num_fields(&value, &numFields), RyuSuccess);
    ASSERT_EQ(numFields, 14);

    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_struct_num_fields(badValue, &numFields), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetStructFieldName) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) WHERE m.name=\"Roma\" RETURN m.description", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    char* fieldName;
    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 0, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "rating");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 1, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "stars");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 2, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "views");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 3, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "release");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 4, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "release_ns");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 5, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "release_ms");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 6, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "release_sec");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 7, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "release_tz");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 8, &fieldName), RyuSuccess);
    ASSERT_STREQ(fieldName, "film");
    ryu_destroy_string(fieldName);

    ASSERT_EQ(ryu_value_get_struct_field_name(&value, 222, &fieldName), RyuError);

    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiValueTest, GetStructFieldValue) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) WHERE m.name=\"Roma\" RETURN m.description", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);

    ryu_value fieldValue;
    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 0, &fieldValue), RyuSuccess);
    ryu_logical_type fieldType;
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_DOUBLE);
    double doubleValue;
    ASSERT_EQ(ryu_value_get_double(&fieldValue, &doubleValue), RyuSuccess);
    ASSERT_DOUBLE_EQ(doubleValue, 1223);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 1, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 2, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_INT64);
    int64_t int64Value;
    ASSERT_EQ(ryu_value_get_int64(&fieldValue, &int64Value), RyuSuccess);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 3, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_TIMESTAMP);
    ryu_timestamp_t timestamp;
    ASSERT_EQ(ryu_value_get_timestamp(&fieldValue, &timestamp), RyuSuccess);
    ASSERT_EQ(timestamp.value, 1297442662000000);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 4, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_TIMESTAMP_NS);
    ryu_timestamp_ns_t timestamp_ns;
    ASSERT_EQ(ryu_value_get_timestamp_ns(&fieldValue, &timestamp_ns), RyuSuccess);
    ASSERT_EQ(timestamp_ns.value, 1297442662123456000);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 5, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_TIMESTAMP_MS);
    ryu_timestamp_ms_t timestamp_ms;
    ASSERT_EQ(ryu_value_get_timestamp_ms(&fieldValue, &timestamp_ms), RyuSuccess);
    ASSERT_EQ(timestamp_ms.value, 1297442662123);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 6, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_TIMESTAMP_SEC);
    ryu_timestamp_sec_t timestamp_sec;
    ASSERT_EQ(ryu_value_get_timestamp_sec(&fieldValue, &timestamp_sec), RyuSuccess);
    ASSERT_EQ(timestamp_sec.value, 1297442662);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 7, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_TIMESTAMP_TZ);
    ryu_timestamp_tz_t timestamp_tz;
    ASSERT_EQ(ryu_value_get_timestamp_tz(&fieldValue, &timestamp_tz), RyuSuccess);
    ASSERT_EQ(timestamp_tz.value, 1297442662123456);
    ryu_data_type_destroy(&fieldType);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 8, &fieldValue), RyuSuccess);
    ryu_value_get_data_type(&fieldValue, &fieldType);
    ASSERT_EQ(ryu_data_type_get_id(&fieldType), RYU_DATE);
    ryu_date_t date;
    ASSERT_EQ(ryu_value_get_date(&fieldValue, &date), RyuSuccess);
    ASSERT_EQ(date.days, 15758);
    ryu_data_type_destroy(&fieldType);
    ryu_value_destroy(&fieldValue);

    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 222, &fieldValue), RyuError);

    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiValueTest, getMapNumFields) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) WHERE m.length = 2544 RETURN m.audience", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_FALSE(ryu_query_result_has_next(&result));
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);

    uint64_t mapFields;
    ASSERT_EQ(ryu_value_get_map_size(&value, &mapFields), RyuSuccess);
    ASSERT_EQ(mapFields, 1);

    ryu_query_result_destroy(&result);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
}

TEST_F(CApiValueTest, getMapKey) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) WHERE m.length = 2544 RETURN m.audience", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_FALSE(ryu_query_result_has_next(&result));
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);

    ryu_value key;
    ASSERT_EQ(ryu_value_get_map_key(&value, 0, &key), RyuSuccess);
    ryu_logical_type keyType;
    ryu_value_get_data_type(&key, &keyType);
    ASSERT_EQ(ryu_data_type_get_id(&keyType), RYU_STRING);
    char* mapName;
    ASSERT_EQ(ryu_value_get_string(&key, &mapName), RyuSuccess);
    ASSERT_STREQ(mapName, "audience1");
    ryu_destroy_string(mapName);
    ryu_data_type_destroy(&keyType);
    ryu_value_destroy(&key);

    ASSERT_EQ(ryu_value_get_map_key(&value, 1, &key), RyuError);
    ryu_query_result_destroy(&result);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
}

TEST_F(CApiValueTest, getMapValue) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) WHERE m.length = 2544 RETURN m.audience", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_FALSE(ryu_query_result_has_next(&result));
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);

    ryu_value mapValue;
    ASSERT_EQ(ryu_value_get_map_value(&value, 0, &mapValue), RyuSuccess);
    ryu_logical_type mapType;
    ryu_value_get_data_type(&mapValue, &mapType);
    ASSERT_EQ(ryu_data_type_get_id(&mapType), RYU_INT64);
    int64_t mapIntValue;
    ASSERT_EQ(ryu_value_get_int64(&mapValue, &mapIntValue), RyuSuccess);
    ASSERT_EQ(mapIntValue, 33);

    ASSERT_EQ(ryu_value_get_map_value(&value, 1, &mapValue), RyuError);

    ryu_data_type_destroy(&mapType);
    ryu_query_result_destroy(&result);
    ryu_value_destroy(&mapValue);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
}

TEST_F(CApiValueTest, getDecimalAsString) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"UNWIND [1] AS A UNWIND [5.7, 8.3, 8.7, 13.7] AS B WITH cast(CAST(A AS DECIMAL) "
               "* "
               "CAST(B AS DECIMAL) AS DECIMAL(18, 1)) AS PROD RETURN COLLECT(PROD) AS RES",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);

    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);

    ryu_logical_type dataType;
    ryu_value_get_data_type(&value, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_LIST);
    uint64_t list_size;
    ASSERT_EQ(ryu_value_get_list_size(&value, &list_size), RyuSuccess);
    ASSERT_EQ(list_size, 4);
    ryu_data_type_destroy(&dataType);

    ryu_value decimal_entry;
    char* decimal_value;
    std::string decimal_string_value;
    ASSERT_EQ(ryu_value_get_list_element(&value, 0, &decimal_entry), RyuSuccess);
    ryu_value_get_data_type(&decimal_entry, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_DECIMAL);
    ASSERT_EQ(ryu_value_get_decimal_as_string(&decimal_entry, &decimal_value), RyuSuccess);
    decimal_string_value = std::string(decimal_value);
    ASSERT_EQ(decimal_string_value, "5.7");
    ryu_destroy_string(decimal_value);
    ryu_data_type_destroy(&dataType);

    ASSERT_EQ(ryu_value_get_list_element(&value, 1, &decimal_entry), RyuSuccess);
    ryu_value_get_data_type(&decimal_entry, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_DECIMAL);
    ASSERT_EQ(ryu_value_get_decimal_as_string(&decimal_entry, &decimal_value), RyuSuccess);
    decimal_string_value = std::string(decimal_value);
    ASSERT_EQ(decimal_string_value, "8.3");
    ryu_destroy_string(decimal_value);
    ryu_data_type_destroy(&dataType);

    ASSERT_EQ(ryu_value_get_list_element(&value, 2, &decimal_entry), RyuSuccess);
    ryu_value_get_data_type(&decimal_entry, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_DECIMAL);
    ASSERT_EQ(ryu_value_get_decimal_as_string(&decimal_entry, &decimal_value), RyuSuccess);
    decimal_string_value = std::string(decimal_value);
    ASSERT_EQ(decimal_string_value, "8.7");
    ryu_destroy_string(decimal_value);
    ryu_data_type_destroy(&dataType);

    ASSERT_EQ(ryu_value_get_list_element(&value, 3, &decimal_entry), RyuSuccess);
    ryu_value_get_data_type(&decimal_entry, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_DECIMAL);
    ASSERT_EQ(ryu_value_get_decimal_as_string(&decimal_entry, &decimal_value), RyuSuccess);
    decimal_string_value = std::string(decimal_value);
    ASSERT_EQ(decimal_string_value, "13.7");
    ryu_destroy_string(decimal_value);
    ryu_data_type_destroy(&dataType);

    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
    ryu_value_destroy(&decimal_entry);
}

TEST_F(CApiValueTest, GetDataType) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.fName, a.isStudent, a.workedHours", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ryu_logical_type dataType;
    ryu_value_get_data_type(&value, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_STRING);
    ryu_data_type_destroy(&dataType);

    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 1, &value), RyuSuccess);
    ryu_value_get_data_type(&value, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_BOOL);
    ryu_data_type_destroy(&dataType);

    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 2, &value), RyuSuccess);
    ryu_value_get_data_type(&value, &dataType);
    ASSERT_EQ(ryu_data_type_get_id(&dataType), RYU_LIST);
    ryu_data_type_destroy(&dataType);
    ryu_value_destroy(&value);

    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiValueTest, GetBool) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.isStudent ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    bool boolValue;
    ASSERT_EQ(ryu_value_get_bool(&value, &boolValue), RyuSuccess);
    ASSERT_TRUE(boolValue);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_bool(badValue, &boolValue), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInt8) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) RETURN r.level ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    int8_t int8Value;
    ASSERT_EQ(ryu_value_get_int8(&value, &int8Value), RyuSuccess);
    ASSERT_EQ(int8Value, 5);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_int8(badValue, &int8Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInt16) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) RETURN r.length ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    int16_t int16Value;
    ASSERT_EQ(ryu_value_get_int16(&value, &int16Value), RyuSuccess);
    ASSERT_EQ(int16Value, 5);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_int16(badValue, &int16Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInt32) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (m:movies) RETURN m.length ORDER BY m.name", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    int32_t int32Value;
    ASSERT_EQ(ryu_value_get_int32(&value, &int32Value), RyuSuccess);
    ASSERT_EQ(int32Value, 298);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_int32(badValue, &int32Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInt64) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a.ID ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    int64_t int64Value;
    ASSERT_EQ(ryu_value_get_int64(&value, &int64Value), RyuSuccess);
    ASSERT_EQ(int64Value, 0);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_int64(badValue, &int64Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetUInt8) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) RETURN r.ulevel ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint8_t uint8Value;
    ASSERT_EQ(ryu_value_get_uint8(&value, &uint8Value), RyuSuccess);
    ASSERT_EQ(uint8Value, 250);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_uint8(badValue, &uint8Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetUInt16) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) RETURN r.ulength ORDER BY "
               "a.ID",
        &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint16_t uint16Value;
    ASSERT_EQ(ryu_value_get_uint16(&value, &uint16Value), RyuSuccess);
    ASSERT_EQ(uint16Value, 33768);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_uint16(badValue, &uint16Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetUInt32) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) "
               "RETURN r.temperature ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint32_t uint32Value;
    ASSERT_EQ(ryu_value_get_uint32(&value, &uint32Value), RyuSuccess);
    ASSERT_EQ(uint32Value, 32800);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_uint32(badValue, &uint32Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetUInt64) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) RETURN r.code ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint64_t uint64Value;
    ASSERT_EQ(ryu_value_get_uint64(&value, &uint64Value), RyuSuccess);
    ASSERT_EQ(uint64Value, 9223372036854775808ull);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_uint64(badValue, &uint64Value), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInt128) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:studyAt]-> (b:organisation) RETURN r.hugedata ORDER BY "
               "a.ID",
        &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    ryu_int128_t int128;
    ASSERT_EQ(ryu_value_get_int128(&value, &int128), RyuSuccess);
    ASSERT_EQ(int128.high, 100000000);
    ASSERT_EQ(int128.low, 211111111);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_int128(badValue, &int128), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, StringToInt128Test) {
    char input[] = "1844674407370955161811111111";
    ryu_int128_t int128_val;
    ASSERT_EQ(ryu_int128_t_from_string(input, &int128_val), RyuSuccess);
    ASSERT_EQ(int128_val.high, 100000000);
    ASSERT_EQ(int128_val.low, 211111111);

    char badInput[] = "this is not a int128";
    ryu_int128_t int128_val2;
    ASSERT_EQ(ryu_int128_t_from_string(badInput, &int128_val2), RyuError);
}

TEST_F(CApiValueTest, Int128ToStringTest) {
    auto int128_val = ryu_int128_t{211111111, 100000000};
    char* str;
    ASSERT_EQ(ryu_int128_t_to_string(int128_val, &str), RyuSuccess);
    ASSERT_STREQ(str, "1844674407370955161811111111");
    ryu_destroy_string(str);
}

TEST_F(CApiValueTest, GetFloat) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.height ORDER BY a.ID", &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    float floatValue;
    ASSERT_EQ(ryu_value_get_float(&value, &floatValue), RyuSuccess);
    ASSERT_FLOAT_EQ(floatValue, 1.731);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_float(badValue, &floatValue), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetDouble) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.eyeSight ORDER BY a.ID", &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    double doubleValue;
    ASSERT_EQ(ryu_value_get_double(&value, &doubleValue), RyuSuccess);
    ASSERT_DOUBLE_EQ(doubleValue, 5.0);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_double(badValue, &doubleValue), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInternalID) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state =
        ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a ORDER BY a.ID", &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ryu_value nodeIDVal;
    ASSERT_EQ(ryu_value_get_struct_field_value(&value, 0 /* internal ID field idx */, &nodeIDVal),
        RyuSuccess);
    ryu_internal_id_t internalID;
    ASSERT_EQ(ryu_value_get_internal_id(&nodeIDVal, &internalID), RyuSuccess);
    ASSERT_EQ(internalID.table_id, 0);
    ASSERT_EQ(internalID.offset, 0);
    ryu_value_destroy(&nodeIDVal);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_internal_id(badValue, &internalID), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetRelVal) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[r:knows]-> (b:person) RETURN r ORDER BY a.ID, b.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value rel;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &rel), RyuSuccess);
    ASSERT_TRUE(rel._is_owned_by_cpp);
    ryu_value relIdVal;
    ASSERT_EQ(ryu_rel_val_get_id_val(&rel, &relIdVal), RyuSuccess);
    ryu_internal_id_t relInternalID;
    ASSERT_EQ(ryu_value_get_internal_id(&relIdVal, &relInternalID), RyuSuccess);
    ASSERT_EQ(relInternalID.table_id, 3);
    ASSERT_EQ(relInternalID.offset, 0);
    ryu_value relSrcIDVal;
    ASSERT_EQ(ryu_rel_val_get_src_id_val(&rel, &relSrcIDVal), RyuSuccess);
    ryu_internal_id_t relSrcID;
    ASSERT_EQ(ryu_value_get_internal_id(&relSrcIDVal, &relSrcID), RyuSuccess);
    ASSERT_EQ(relSrcID.table_id, 0);
    ASSERT_EQ(relSrcID.offset, 0);
    ryu_value relDstIDVal;
    ASSERT_EQ(ryu_rel_val_get_dst_id_val(&rel, &relDstIDVal), RyuSuccess);
    ryu_internal_id_t relDstID;
    ASSERT_EQ(ryu_value_get_internal_id(&relDstIDVal, &relDstID), RyuSuccess);
    ASSERT_EQ(relDstID.table_id, 0);
    ASSERT_EQ(relDstID.offset, 1);
    ryu_value relLabel;
    ASSERT_EQ(ryu_rel_val_get_label_val(&rel, &relLabel), RyuSuccess);
    char* relLabelStr;
    ASSERT_EQ(ryu_value_get_string(&relLabel, &relLabelStr), RyuSuccess);
    ASSERT_STREQ(relLabelStr, "knows");
    uint64_t propertiesSize;
    ASSERT_EQ(ryu_rel_val_get_property_size(&rel, &propertiesSize), RyuSuccess);
    ASSERT_EQ(propertiesSize, 7);
    ryu_destroy_string(relLabelStr);
    ryu_value_destroy(&relLabel);
    ryu_value_destroy(&relIdVal);
    ryu_value_destroy(&relSrcIDVal);
    ryu_value_destroy(&relDstIDVal);
    ryu_value_destroy(&rel);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_rel_val_get_src_id_val(badValue, &relSrcIDVal), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetDate) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.birthdate ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ryu_date_t date;
    ASSERT_EQ(ryu_value_get_date(&value, &date), RyuSuccess);
    ASSERT_EQ(date.days, -25567);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_date(badValue, &date), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetTimestamp) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.registerTime ORDER BY a.ID", &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ryu_timestamp_t timestamp;
    ASSERT_EQ(ryu_value_get_timestamp(&value, &timestamp), RyuSuccess);
    ASSERT_EQ(timestamp.value, 1313839530000000);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_timestamp(badValue, &timestamp), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetInterval) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.lastJobDuration ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ryu_interval_t interval;
    ASSERT_EQ(ryu_value_get_interval(&value, &interval), RyuSuccess);
    ASSERT_EQ(interval.months, 36);
    ASSERT_EQ(interval.days, 2);
    ASSERT_EQ(interval.micros, 46920000000);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_interval(badValue, &interval), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetString) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a.fName ORDER BY a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    char* str;
    ASSERT_EQ(ryu_value_get_string(&value, &str), RyuSuccess);
    ASSERT_STREQ(str, "Alice");
    ryu_destroy_string(str);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_int32(123);
    ASSERT_EQ(ryu_value_get_string(badValue, &str), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetBlob) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, (char*)R"(RETURN BLOB('\xAA\xBB\xCD\x1A');)", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    uint8_t* blob;
    ASSERT_EQ(ryu_value_get_blob(&value, &blob), RyuSuccess);
    ASSERT_EQ(blob[0], 0xAA);
    ASSERT_EQ(blob[1], 0xBB);
    ASSERT_EQ(blob[2], 0xCD);
    ASSERT_EQ(blob[3], 0x1A);
    ASSERT_EQ(blob[4], 0x00);
    ryu_destroy_blob(blob);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_blob(badValue, &blob), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetUUID) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)R"(RETURN UUID("A0EEBC99-9C0B-4EF8-BB6D-6BB9BD380A11");)", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ryu_flat_tuple_get_value(&flatTuple, 0, &value);
    ASSERT_TRUE(value._is_owned_by_cpp);
    ASSERT_FALSE(ryu_value_is_null(&value));
    char* str;
    ASSERT_EQ(ryu_value_get_uuid(&value, &str), RyuSuccess);
    ASSERT_STREQ(str, "a0eebc99-9c0b-4ef8-bb6d-6bb9bd380a11");
    ryu_destroy_string(str);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_value_get_uuid(badValue, &str), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, ToSting) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) RETURN a.fName, a.isStudent, a.workedHours ORDER BY "
               "a.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));

    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);

    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    char* str = ryu_value_to_string(&value);
    ASSERT_STREQ(str, "Alice");
    ryu_destroy_string(str);

    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 1, &value), RyuSuccess);
    str = ryu_value_to_string(&value);
    ASSERT_STREQ(str, "True");
    ryu_destroy_string(str);

    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 2, &value), RyuSuccess);
    str = ryu_value_to_string(&value);
    ASSERT_STREQ(str, "[10,5]");
    ryu_destroy_string(str);
    ryu_value_destroy(&value);

    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiValueTest, NodeValGetLabelVal) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state =
        ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));

    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value nodeVal;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &nodeVal), RyuSuccess);
    ryu_value labelVal;
    ASSERT_EQ(ryu_node_val_get_label_val(&nodeVal, &labelVal), RyuSuccess);
    char* labelStr;
    ASSERT_EQ(ryu_value_get_string(&labelVal, &labelStr), RyuSuccess);
    ASSERT_STREQ(labelStr, "person");
    ryu_destroy_string(labelStr);
    ryu_value_destroy(&labelVal);
    ryu_value_destroy(&nodeVal);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_node_val_get_label_val(badValue, &labelVal), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, NodeValGetID) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state =
        ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));

    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value nodeVal;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &nodeVal), RyuSuccess);
    ryu_value nodeIDVal;
    ASSERT_EQ(ryu_node_val_get_id_val(&nodeVal, &nodeIDVal), RyuSuccess);
    ASSERT_NE(nodeIDVal._value, nullptr);
    ryu_internal_id_t internalID;
    ASSERT_EQ(ryu_value_get_internal_id(&nodeIDVal, &internalID), RyuSuccess);
    ASSERT_EQ(internalID.table_id, 0);
    ASSERT_EQ(internalID.offset, 0);
    ryu_value_destroy(&nodeIDVal);
    ryu_value_destroy(&nodeVal);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_node_val_get_id_val(badValue, &nodeIDVal), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, NodeValGetLabelName) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state =
        ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));

    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value nodeVal;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &nodeVal), RyuSuccess);
    ryu_value labelVal;
    ASSERT_EQ(ryu_node_val_get_label_val(&nodeVal, &labelVal), RyuSuccess);
    char* labelStr;
    ASSERT_EQ(ryu_value_get_string(&labelVal, &labelStr), RyuSuccess);
    ASSERT_STREQ(labelStr, "person");
    ryu_destroy_string(labelStr);
    ryu_value_destroy(&labelVal);
    ryu_value_destroy(&nodeVal);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_node_val_get_label_val(badValue, &labelVal), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, NodeValGetProperty) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state =
        ryu_connection_query(connection, (char*)"MATCH (a:person) RETURN a ORDER BY a.ID", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value node;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &node), RyuSuccess);
    char* propertyName;
    ASSERT_EQ(ryu_node_val_get_property_name_at(&node, 0, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "ID");
    ryu_destroy_string(propertyName);
    ASSERT_EQ(ryu_node_val_get_property_name_at(&node, 1, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "fName");
    ryu_destroy_string(propertyName);
    ASSERT_EQ(ryu_node_val_get_property_name_at(&node, 2, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "gender");
    ryu_destroy_string(propertyName);
    ASSERT_EQ(ryu_node_val_get_property_name_at(&node, 3, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "isStudent");
    ryu_destroy_string(propertyName);

    ryu_value propertyValue;
    ASSERT_EQ(ryu_node_val_get_property_value_at(&node, 0, &propertyValue), RyuSuccess);
    int64_t propertyValueID;
    ASSERT_EQ(ryu_value_get_int64(&propertyValue, &propertyValueID), RyuSuccess);
    ASSERT_EQ(propertyValueID, 0);
    ASSERT_EQ(ryu_node_val_get_property_value_at(&node, 1, &propertyValue), RyuSuccess);
    char* propertyValuefName;
    ASSERT_EQ(ryu_value_get_string(&propertyValue, &propertyValuefName), RyuSuccess);
    ASSERT_STREQ(propertyValuefName, "Alice");
    ryu_destroy_string(propertyValuefName);
    ASSERT_EQ(ryu_node_val_get_property_value_at(&node, 2, &propertyValue), RyuSuccess);
    int64_t propertyValueGender;
    ASSERT_EQ(ryu_value_get_int64(&propertyValue, &propertyValueGender), RyuSuccess);
    ASSERT_EQ(propertyValueGender, 1);
    ASSERT_EQ(ryu_node_val_get_property_value_at(&node, 3, &propertyValue), RyuSuccess);
    bool propertyValueIsStudent;
    ASSERT_EQ(ryu_value_get_bool(&propertyValue, &propertyValueIsStudent), RyuSuccess);
    ASSERT_EQ(propertyValueIsStudent, true);
    ryu_value_destroy(&propertyValue);

    ryu_value_destroy(&node);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_node_val_get_property_name_at(badValue, 0, &propertyName), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, NodeValToString) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, (char*)"MATCH (b:organisation) RETURN b ORDER BY b.ID",
        &result);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value node;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &node), RyuSuccess);
    ASSERT_TRUE(node._is_owned_by_cpp);

    char* str = ryu_value_to_string(&node);
    ASSERT_STREQ(str,
        "{_ID: 1:0, _LABEL: organisation, ID: 1, name: ABFsUni, orgCode: 325, mark: 3.700000, "
        "score: -2, history: 10 years 5 months 13 hours 24 us, licenseValidInterval: 3 years "
        "5 days, rating: 1.000000, state: {revenue: 138, location: ['toronto','montr,eal'], "
        "stock: {price: [96,56], volume: 1000}}, info: 3.120000}");
    ryu_destroy_string(str);

    ryu_value_destroy(&node);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiValueTest, RelValGetProperty) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[e:workAt]-> (b:organisation) RETURN e ORDER BY a.ID, b.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value rel;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &rel), RyuSuccess);
    ASSERT_TRUE(rel._is_owned_by_cpp);
    uint64_t propertiesSize;
    ASSERT_EQ(ryu_rel_val_get_property_size(&rel, &propertiesSize), RyuSuccess);
    ASSERT_EQ(propertiesSize, 3);

    char* propertyName;
    ASSERT_EQ(ryu_rel_val_get_property_name_at(&rel, 0, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "year");
    ryu_destroy_string(propertyName);

    ASSERT_EQ(ryu_rel_val_get_property_name_at(&rel, 1, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "grading");
    ryu_destroy_string(propertyName);
    ASSERT_EQ(ryu_rel_val_get_property_name_at(&rel, 2, &propertyName), RyuSuccess);
    ASSERT_STREQ(propertyName, "rating");
    ryu_destroy_string(propertyName);

    ryu_value propertyValue;
    ASSERT_EQ(ryu_rel_val_get_property_value_at(&rel, 0, &propertyValue), RyuSuccess);
    int64_t propertyValueYear;
    ASSERT_EQ(ryu_value_get_int64(&propertyValue, &propertyValueYear), RyuSuccess);
    ASSERT_EQ(propertyValueYear, 2015);

    ASSERT_EQ(ryu_rel_val_get_property_value_at(&rel, 1, &propertyValue), RyuSuccess);
    ryu_value listValue;
    ASSERT_EQ(ryu_value_get_list_element(&propertyValue, 0, &listValue), RyuSuccess);
    double listValueGrading;
    ASSERT_EQ(ryu_value_get_double(&listValue, &listValueGrading), RyuSuccess);
    ASSERT_DOUBLE_EQ(listValueGrading, 3.8);
    ASSERT_EQ(ryu_value_get_list_element(&propertyValue, 1, &listValue), RyuSuccess);
    ASSERT_EQ(ryu_value_get_double(&listValue, &listValueGrading), RyuSuccess);
    ASSERT_DOUBLE_EQ(listValueGrading, 2.5);
    ryu_value_destroy(&listValue);

    ASSERT_EQ(ryu_rel_val_get_property_value_at(&rel, 2, &propertyValue), RyuSuccess);
    float propertyValueRating;
    ASSERT_EQ(ryu_value_get_float(&propertyValue, &propertyValueRating), RyuSuccess);
    ASSERT_FLOAT_EQ(propertyValueRating, 8.2);
    ryu_value_destroy(&propertyValue);

    ryu_value_destroy(&rel);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_rel_val_get_property_name_at(badValue, 0, &propertyName), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, RelValToString) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        (char*)"MATCH (a:person) -[e:workAt]-> (b:organisation) RETURN e ORDER BY a.ID, b.ID",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value rel;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &rel), RyuSuccess);
    ASSERT_TRUE(rel._is_owned_by_cpp);
    char* str;
    ASSERT_EQ(ryu_rel_val_to_string(&rel, &str), RyuSuccess);
    ASSERT_STREQ(str, "(0:2)-{_LABEL: workAt, _ID: 7:0, year: 2015, grading: [3.800000,2.500000], "
                      "rating: 8.200000}->(1:1)");
    ryu_destroy_string(str);
    ryu_value_destroy(&rel);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);

    ryu_value* badValue = ryu_value_create_string((char*)"abcdefg");
    ASSERT_EQ(ryu_rel_val_to_string(badValue, &str), RyuError);
    ryu_value_destroy(badValue);
}

TEST_F(CApiValueTest, GetTmFromNonStandardTimestamp) {
    ryu_timestamp_ns_t timestamp_ns = ryu_timestamp_ns_t{17515323532900000};
    ryu_timestamp_ms_t timestamp_ms = ryu_timestamp_ms_t{1012323435341};
    ryu_timestamp_sec_t timestamp_sec = ryu_timestamp_sec_t{1432135648};
    ryu_timestamp_tz_t timestamp_tz = ryu_timestamp_tz_t{771513532900000};
    struct tm tm;
    ASSERT_EQ(ryu_timestamp_ns_to_tm(timestamp_ns, &tm), RyuSuccess);
    ASSERT_EQ(tm.tm_year, 70);
    ASSERT_EQ(tm.tm_mon, 6);
    ASSERT_EQ(tm.tm_mday, 22);
    ASSERT_EQ(tm.tm_hour, 17);
    ASSERT_EQ(tm.tm_min, 22);
    ASSERT_EQ(tm.tm_sec, 3);
    ASSERT_EQ(ryu_timestamp_ms_to_tm(timestamp_ms, &tm), RyuSuccess);
    ASSERT_EQ(tm.tm_year, 102);
    ASSERT_EQ(tm.tm_mon, 0);
    ASSERT_EQ(tm.tm_mday, 29);
    ASSERT_EQ(tm.tm_hour, 16);
    ASSERT_EQ(tm.tm_min, 57);
    ASSERT_EQ(tm.tm_sec, 15);
    ASSERT_EQ(ryu_timestamp_sec_to_tm(timestamp_sec, &tm), RyuSuccess);
    ASSERT_EQ(tm.tm_year, 115);
    ASSERT_EQ(tm.tm_mon, 4);
    ASSERT_EQ(tm.tm_mday, 20);
    ASSERT_EQ(tm.tm_hour, 15);
    ASSERT_EQ(tm.tm_min, 27);
    ASSERT_EQ(tm.tm_sec, 28);
    ASSERT_EQ(ryu_timestamp_tz_to_tm(timestamp_tz, &tm), RyuSuccess);
    ASSERT_EQ(tm.tm_year, 94);
    ASSERT_EQ(tm.tm_mon, 5);
    ASSERT_EQ(tm.tm_mday, 13);
    ASSERT_EQ(tm.tm_hour, 13);
    ASSERT_EQ(tm.tm_min, 18);
    ASSERT_EQ(tm.tm_sec, 52);
}

TEST_F(CApiValueTest, GetTmFromTimestamp) {
    ryu_timestamp_t timestamp = ryu_timestamp_t{171513532900000};
    struct tm tm;
    ASSERT_EQ(ryu_timestamp_to_tm(timestamp, &tm), RyuSuccess);
    ASSERT_EQ(tm.tm_year, 75);
    ASSERT_EQ(tm.tm_mon, 5);
    ASSERT_EQ(tm.tm_mday, 9);
    ASSERT_EQ(tm.tm_hour, 2);
    ASSERT_EQ(tm.tm_min, 38);
    ASSERT_EQ(tm.tm_sec, 52);
}

TEST_F(CApiValueTest, GetTmFromDate) {
    ryu_date_t date = ryu_date_t{-255};
    struct tm tm;
    ASSERT_EQ(ryu_date_to_tm(date, &tm), RyuSuccess);
    ASSERT_EQ(tm.tm_year, 69);
    ASSERT_EQ(tm.tm_mon, 3);
    ASSERT_EQ(tm.tm_mday, 21);
    ASSERT_EQ(tm.tm_hour, 0);
    ASSERT_EQ(tm.tm_min, 0);
    ASSERT_EQ(tm.tm_sec, 0);
}

TEST_F(CApiValueTest, GetTimestampFromTm) {
    struct tm tm;
    tm.tm_year = 75;
    tm.tm_mon = 5;
    tm.tm_mday = 9;
    tm.tm_hour = 2;
    tm.tm_min = 38;
    tm.tm_sec = 52;
    ryu_timestamp_t timestamp;
    ASSERT_EQ(ryu_timestamp_from_tm(tm, &timestamp), RyuSuccess);
    ASSERT_EQ(timestamp.value, 171513532000000);
}

TEST_F(CApiValueTest, GetNonStandardTimestampFromTm) {
    struct tm tm;
    tm.tm_year = 70;
    tm.tm_mon = 6;
    tm.tm_mday = 22;
    tm.tm_hour = 17;
    tm.tm_min = 22;
    tm.tm_sec = 3;
    ryu_timestamp_ns_t timestamp_ns;
    ASSERT_EQ(ryu_timestamp_ns_from_tm(tm, &timestamp_ns), RyuSuccess);
    ASSERT_EQ(timestamp_ns.value, 17515323000000000);
    tm.tm_year = 102;
    tm.tm_mon = 0;
    tm.tm_mday = 29;
    tm.tm_hour = 16;
    tm.tm_min = 57;
    tm.tm_sec = 15;
    ryu_timestamp_ms_t timestamp_ms;
    ASSERT_EQ(ryu_timestamp_ms_from_tm(tm, &timestamp_ms), RyuSuccess);
    ASSERT_EQ(timestamp_ms.value, 1012323435000);
    tm.tm_year = 115;
    tm.tm_mon = 4;
    tm.tm_mday = 20;
    tm.tm_hour = 15;
    tm.tm_min = 27;
    tm.tm_sec = 28;
    ryu_timestamp_sec_t timestamp_sec;
    ASSERT_EQ(ryu_timestamp_sec_from_tm(tm, &timestamp_sec), RyuSuccess);
    ASSERT_EQ(timestamp_sec.value, 1432135648);
    tm.tm_year = 94;
    tm.tm_mon = 5;
    tm.tm_mday = 13;
    tm.tm_hour = 13;
    tm.tm_min = 18;
    tm.tm_sec = 52;
    ryu_timestamp_tz_t timestamp_tz;
    ASSERT_EQ(ryu_timestamp_tz_from_tm(tm, &timestamp_tz), RyuSuccess);
    ASSERT_EQ(timestamp_tz.value, 771513532000000);
}

TEST_F(CApiValueTest, GetDateFromTm) {
    struct tm tm;
    tm.tm_year = 69;
    tm.tm_mon = 3;
    tm.tm_mday = 21;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    ryu_date_t date;
    ASSERT_EQ(ryu_date_from_tm(tm, &date), RyuSuccess);
    ASSERT_EQ(date.days, -255);
}

TEST_F(CApiValueTest, GetDateFromString) {
    char input[] = "1969-04-21";
    ryu_date_t date;
    ASSERT_EQ(ryu_date_from_string(input, &date), RyuSuccess);
    ASSERT_EQ(date.days, -255);

    char badInput[] = "this is not a date";
    ASSERT_EQ(ryu_date_from_string(badInput, &date), RyuError);
}

TEST_F(CApiValueTest, GetStringFromDate) {
    ryu_date_t date = ryu_date_t{-255};
    char* str;
    ASSERT_EQ(ryu_date_to_string(date, &str), RyuSuccess);
    ASSERT_STREQ(str, "1969-04-21");
    ryu_destroy_string(str);
}

TEST_F(CApiValueTest, GetDifftimeFromInterval) {
    ryu_interval_t interval = ryu_interval_t{36, 2, 46920000000};
    double difftime;
    ryu_interval_to_difftime(interval, &difftime);
    ASSERT_DOUBLE_EQ(difftime, 93531720);
}

TEST_F(CApiValueTest, GetIntervalFromDifftime) {
    double difftime = 211110160.479;
    ryu_interval_t interval;
    ryu_interval_from_difftime(difftime, &interval);
    ASSERT_EQ(interval.months, 81);
    ASSERT_EQ(interval.days, 13);
    ASSERT_EQ(interval.micros, 34960479000);
}
