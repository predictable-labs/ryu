#include "c_api_test/c_api_test.h"

using namespace ryu::common;
using namespace ryu::main;
using namespace ryu::testing;

class CApiFlatTupleTest : public CApiTest {
public:
    std::string getInputDir() override { return TestHelper::appendRyuRootPath("dataset/tinysnb/"); }
};

TEST_F(CApiFlatTupleTest, GetValue) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        "MATCH (a:person) RETURN a.fName, a.age, a.height ORDER BY a.fName LIMIT 1", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    ryu_value value;
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 0, &value), RyuSuccess);
    ASSERT_NE(value._value, nullptr);
    auto valueCpp = static_cast<Value*>(value._value);
    ASSERT_NE(valueCpp, nullptr);
    ASSERT_EQ(valueCpp->getDataType().getLogicalTypeID(), LogicalTypeID::STRING);
    ASSERT_EQ(valueCpp->getValue<std::string>(), "Alice");
    ryu_value_destroy(&value);
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 1, &value), RyuSuccess);
    ASSERT_NE(value._value, nullptr);
    valueCpp = static_cast<Value*>(value._value);
    ASSERT_NE(valueCpp, nullptr);
    ASSERT_EQ(valueCpp->getDataType().getLogicalTypeID(), LogicalTypeID::INT64);
    ASSERT_EQ(valueCpp->getValue<int64_t>(), 35);
    ryu_value_destroy(&value);
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 2, &value), RyuSuccess);
    ASSERT_NE(value._value, nullptr);
    valueCpp = static_cast<Value*>(value._value);
    ASSERT_NE(valueCpp, nullptr);
    ASSERT_EQ(valueCpp->getDataType().getLogicalTypeID(), LogicalTypeID::FLOAT);
    ASSERT_FLOAT_EQ(valueCpp->getValue<float>(), 1.731);
    ryu_value_destroy(&value);
    ASSERT_EQ(ryu_flat_tuple_get_value(&flatTuple, 222, &value), RyuError);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiFlatTupleTest, ToString) {
    ryu_query_result result;
    ryu_flat_tuple flatTuple;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        "MATCH (a:person) RETURN a.fName, a.age, a.height ORDER BY a.fName LIMIT 1", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    state = ryu_query_result_get_next(&result, &flatTuple);
    ASSERT_EQ(state, RyuSuccess);
    auto columnWidths = (uint32_t*)malloc(3 * sizeof(uint32_t));
    columnWidths[0] = 10;
    columnWidths[1] = 5;
    columnWidths[2] = 10;
    char* str = ryu_flat_tuple_to_string(&flatTuple);
    ASSERT_EQ(std::string(str), "Alice|35|1.731000\n");
    ryu_destroy_string(str);
    free(columnWidths);
    ryu_flat_tuple_destroy(&flatTuple);
    ryu_query_result_destroy(&result);
}
