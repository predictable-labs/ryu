#include <fstream>

#include "c_api_test/c_api_test.h"

using namespace ryu::main;
using namespace ryu::common;
using namespace ryu::processor;
using namespace ryu::testing;

class CApiQueryResultTest : public CApiTest {
public:
    std::string getInputDir() override { return TestHelper::appendRyuRootPath("dataset/tinysnb/"); }
};

static ryu_value* copy_flat_tuple(ryu_flat_tuple* tuple, uint32_t tupleLen) {
    ryu_value* ret = (ryu_value*)malloc(sizeof(ryu_value) * tupleLen);
    for (uint32_t i = 0; i < tupleLen; i++) {
        ryu_flat_tuple_get_value(tuple, i, &ret[i]);
    }
    return ret;
}

TEST_F(CApiQueryResultTest, GetNextExample) {
    auto conn = getConnection();

    ryu_query_result result;
    ryu_connection_query(conn, "MATCH (p:person) RETURN p.*", &result);

    uint64_t num_tuples = ryu_query_result_get_num_tuples(&result);
    ryu_value** tuples = (ryu_value**)malloc(sizeof(ryu_value*) * num_tuples);
    for (uint64_t i = 0; i < num_tuples; ++i) {
        ryu_flat_tuple tuple;
        ryu_query_result_get_next(&result, &tuple);
        tuples[i] = copy_flat_tuple(&tuple, ryu_query_result_get_num_columns(&result));
        ryu_flat_tuple_destroy(&tuple);
    }

    for (uint64_t i = 0; i < num_tuples; ++i) {
        for (uint64_t j = 0; j < ryu_query_result_get_num_columns(&result); ++j) {
            ASSERT_FALSE(ryu_value_is_null(&tuples[i][j]));
            ryu_value_destroy(&tuples[i][j]);
        }
        free(tuples[i]);
    }

    free((void*)tuples);

    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, GetErrorMessage) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "MATCH (a:person) RETURN COUNT(*)", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    char* errorMessage = ryu_query_result_get_error_message(&result);
    ryu_query_result_destroy(&result);

    state = ryu_connection_query(connection, "MATCH (a:personnnn) RETURN COUNT(*)", &result);
    ASSERT_EQ(state, RyuError);
    ASSERT_FALSE(ryu_query_result_is_success(&result));
    errorMessage = ryu_query_result_get_error_message(&result);
    ASSERT_EQ(std::string(errorMessage), "Binder exception: Table personnnn does not exist.");
    ryu_query_result_destroy(&result);
    ryu_destroy_string(errorMessage);
}

TEST_F(CApiQueryResultTest, ToString) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "MATCH (a:person) RETURN COUNT(*)", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    char* str_repr = ryu_query_result_to_string(&result);
    ASSERT_EQ(state, RyuSuccess);
    ryu_destroy_string(str_repr);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, GetNumColumns) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "MATCH (a:person) RETURN a.fName, a.age, a.height",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 3);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, GetColumnName) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "MATCH (a:person) RETURN a.fName, a.age, a.height",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    char* columnName;
    ASSERT_EQ(ryu_query_result_get_column_name(&result, 0, &columnName), RyuSuccess);
    ASSERT_EQ(std::string(columnName), "a.fName");
    ryu_destroy_string(columnName);
    ASSERT_EQ(ryu_query_result_get_column_name(&result, 1, &columnName), RyuSuccess);
    ASSERT_EQ(std::string(columnName), "a.age");
    ryu_destroy_string(columnName);
    ASSERT_EQ(ryu_query_result_get_column_name(&result, 2, &columnName), RyuSuccess);
    ASSERT_EQ(std::string(columnName), "a.height");
    ryu_destroy_string(columnName);
    ASSERT_EQ(ryu_query_result_get_column_name(&result, 222, &columnName), RyuError);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, GetColumnDataType) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "MATCH (a:person) RETURN a.fName, a.age, a.height",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ryu_logical_type type;
    ASSERT_EQ(ryu_query_result_get_column_data_type(&result, 0, &type), RyuSuccess);
    auto typeCpp = (LogicalType*)(type._data_type);
    ASSERT_EQ(typeCpp->getLogicalTypeID(), LogicalTypeID::STRING);
    ryu_data_type_destroy(&type);
    ASSERT_EQ(ryu_query_result_get_column_data_type(&result, 1, &type), RyuSuccess);
    typeCpp = (LogicalType*)(type._data_type);
    ASSERT_EQ(typeCpp->getLogicalTypeID(), LogicalTypeID::INT64);
    ryu_data_type_destroy(&type);
    ASSERT_EQ(ryu_query_result_get_column_data_type(&result, 2, &type), RyuSuccess);
    typeCpp = (LogicalType*)(type._data_type);
    ASSERT_EQ(typeCpp->getLogicalTypeID(), LogicalTypeID::FLOAT);
    ryu_data_type_destroy(&type);
    ASSERT_EQ(ryu_query_result_get_column_data_type(&result, 222, &type), RyuError);
    ryu_query_result_destroy(&result);
}

// TODO(Guodong): Fix this test by adding support of STRUCT in arrow table export.
// TEST_F(CApiQueryResultTest, GetArrowSchema) {
//    auto connection = getConnection();
//    auto result = ryu_connection_query(
//        connection, "MATCH (p:person)-[k:knows]-(q:person) RETURN p.fName, k, q.fName");
//    ASSERT_TRUE(ryu_query_result_is_success(result));
//    auto schema = ryu_query_result_get_arrow_schema(result);
//    ASSERT_STREQ(schema.name, "ryu_query_result");
//    ASSERT_EQ(schema.n_children, 3);
//    ASSERT_STREQ(schema.children[0]->name, "p.fName");
//    ASSERT_STREQ(schema.children[1]->name, "k");
//    ASSERT_STREQ(schema.children[2]->name, "q.fName");
//
//    schema.release(&schema);
//    ryu_query_result_destroy(result);
//}

TEST_F(CApiQueryResultTest, GetQuerySummary) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "MATCH (a:person) RETURN a.fName, a.age, a.height",
        &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ryu_query_summary summary;
    state = ryu_query_result_get_query_summary(&result, &summary);
    ASSERT_EQ(state, RyuSuccess);
    auto compilingTime = ryu_query_summary_get_compiling_time(&summary);
    ASSERT_GT(compilingTime, 0);
    auto executionTime = ryu_query_summary_get_execution_time(&summary);
    ASSERT_GT(executionTime, 0);
    ryu_query_summary_destroy(&summary);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, GetNext) {
    ryu_query_result result;
    ryu_flat_tuple row;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        "MATCH (a:person) RETURN a.fName, a.age ORDER BY a.fName", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));

    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &row);
    ASSERT_EQ(state, RyuSuccess);
    auto flatTupleCpp = (FlatTuple*)(row._flat_tuple);
    ASSERT_EQ(flatTupleCpp->getValue(0)->getValue<std::string>(), "Alice");
    ASSERT_EQ(flatTupleCpp->getValue(1)->getValue<int64_t>(), 35);

    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &row);
    ASSERT_EQ(state, RyuSuccess);
    flatTupleCpp = (FlatTuple*)(row._flat_tuple);
    ASSERT_EQ(flatTupleCpp->getValue(0)->getValue<std::string>(), "Bob");
    ASSERT_EQ(flatTupleCpp->getValue(1)->getValue<int64_t>(), 30);
    ryu_flat_tuple_destroy(&row);

    while (ryu_query_result_has_next(&result)) {
        ryu_query_result_get_next(&result, &row);
    }
    ASSERT_FALSE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &row);
    ASSERT_EQ(state, RyuError);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, ResetIterator) {
    ryu_query_result result;
    ryu_flat_tuple row;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection,
        "MATCH (a:person) RETURN a.fName, a.age ORDER BY a.fName", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));

    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &row);
    ASSERT_EQ(state, RyuSuccess);
    auto flatTupleCpp = (FlatTuple*)(row._flat_tuple);
    ASSERT_EQ(flatTupleCpp->getValue(0)->getValue<std::string>(), "Alice");
    ASSERT_EQ(flatTupleCpp->getValue(1)->getValue<int64_t>(), 35);

    ryu_query_result_reset_iterator(&result);

    ASSERT_TRUE(ryu_query_result_has_next(&result));
    state = ryu_query_result_get_next(&result, &row);
    ASSERT_EQ(state, RyuSuccess);
    flatTupleCpp = (FlatTuple*)(row._flat_tuple);
    ASSERT_EQ(flatTupleCpp->getValue(0)->getValue<std::string>(), "Alice");
    ASSERT_EQ(flatTupleCpp->getValue(1)->getValue<int64_t>(), 35);
    ryu_flat_tuple_destroy(&row);

    ryu_query_result_destroy(&result);
}

TEST_F(CApiQueryResultTest, MultipleQuery) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_query(connection, "return 1; return 2; return 3;", &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));

    char* str = ryu_query_result_to_string(&result);
    ASSERT_EQ(std::string(str), "1\n1\n");
    ryu_destroy_string(str);

    ASSERT_TRUE(ryu_query_result_has_next_query_result(&result));
    ryu_query_result next_query_result;
    ASSERT_EQ(ryu_query_result_get_next_query_result(&result, &next_query_result), RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&next_query_result));
    str = ryu_query_result_to_string(&next_query_result);
    ASSERT_EQ(std::string(str), "2\n2\n");
    ryu_destroy_string(str);
    ryu_query_result_destroy(&next_query_result);

    ASSERT_EQ(ryu_query_result_get_next_query_result(&result, &next_query_result), RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&next_query_result));
    str = ryu_query_result_to_string(&next_query_result);
    ASSERT_EQ(std::string(str), "3\n3\n");
    ryu_destroy_string(str);

    ASSERT_FALSE(ryu_query_result_has_next_query_result(&result));
    ASSERT_EQ(ryu_query_result_get_next_query_result(&result, &next_query_result), RyuError);
    ryu_query_result_destroy(&next_query_result);

    ryu_query_result_destroy(&result);
}
