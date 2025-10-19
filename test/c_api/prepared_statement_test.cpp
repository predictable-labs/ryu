#include "c_api_test/c_api_test.h"

using namespace ryu::main;
using namespace ryu::testing;

class CApiPreparedStatementTest : public CApiTest {
public:
    std::string getInputDir() override { return TestHelper::appendRyuRootPath("dataset/tinysnb/"); }
};

TEST_F(CApiPreparedStatementTest, IsSuccess) {
    ryu_prepared_statement preparedStatement;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(preparedStatement._prepared_statement, nullptr);
    ASSERT_TRUE(ryu_prepared_statement_is_success(&preparedStatement));
    ryu_prepared_statement_destroy(&preparedStatement);

    query = "MATCH (a:personnnn) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(preparedStatement._prepared_statement, nullptr);
    ASSERT_FALSE(ryu_prepared_statement_is_success(&preparedStatement));
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, GetErrorMessage) {
    ryu_prepared_statement preparedStatement;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(preparedStatement._prepared_statement, nullptr);
    ASSERT_EQ(ryu_prepared_statement_get_error_message(&preparedStatement), nullptr);
    ryu_prepared_statement_destroy(&preparedStatement);

    query = "MATCH (a:personnnn) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(preparedStatement._prepared_statement, nullptr);
    char* message = ryu_prepared_statement_get_error_message(&preparedStatement);
    ASSERT_EQ(std::string(message), "Binder exception: Table personnnn does not exist.");
    ryu_prepared_statement_destroy(&preparedStatement);
    ryu_destroy_string(message);
}

TEST_F(CApiPreparedStatementTest, BindBool) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_bool(&preparedStatement, "1", true), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 3);
    ryu_query_result_destroy(&result);
    // Bind a different parameter
    ASSERT_EQ(ryu_prepared_statement_bind_bool(&preparedStatement, "1", false), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    resultCpp = static_cast<QueryResult*>(result._query_result);
    tuple = resultCpp->getNext();
    value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 5);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindInt64) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.age > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_int64(&preparedStatement, "1", 30), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 4);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindInt32) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:movies) WHERE a.length > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_int32(&preparedStatement, "1", 200), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindInt16) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query =
        "MATCH (a:person) -[s:studyAt]-> (b:organisation) WHERE s.length > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_int16(&preparedStatement, "1", 10), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindInt8) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query =
        "MATCH (a:person) -[s:studyAt]-> (b:organisation) WHERE s.level > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_int8(&preparedStatement, "1", 3), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindUInt64) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query =
        "MATCH (a:person) -[s:studyAt]-> (b:organisation) WHERE s.code > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(ryu_prepared_statement_bind_uint64(&preparedStatement, "1", 100), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindUInt32) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query =
        "MATCH (a:person) -[s:studyAt]-> (b:organisation) WHERE s.temperature> $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_uint32(&preparedStatement, "1", 10), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindUInt16) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query =
        "MATCH (a:person) -[s:studyAt]-> (b:organisation) WHERE s.ulength> $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_uint16(&preparedStatement, "1", 100), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindUInt8) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query =
        "MATCH (a:person) -[s:studyAt]-> (b:organisation) WHERE s.ulevel> $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_uint8(&preparedStatement, "1", 14), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 2);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindDouble) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.eyeSight > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_double(&preparedStatement, "1", 4.5), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 7);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindFloat) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.height < $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_float(&preparedStatement, "1", 1.0), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 1);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindString) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.fName = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_prepared_statement_is_success(&preparedStatement));
    ASSERT_EQ(ryu_prepared_statement_bind_string(&preparedStatement, "1", "Alice"), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 1);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindDate) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.birthdate > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_prepared_statement_is_success(&preparedStatement));
    auto date = ryu_date_t{0};
    ASSERT_EQ(ryu_prepared_statement_bind_date(&preparedStatement, "1", date), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 4);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindTimestamp) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.registerTime > $1 and cast(a.registerTime, "
                 "\"timestamp_ns\") > $2 and cast(a.registerTime, \"timestamp_ms\") > "
                 "$3 and cast(a.registerTime, \"timestamp_sec\") > $4 and cast(a.registerTime, "
                 "\"timestamp_tz\") > $5 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_prepared_statement_is_success(&preparedStatement));
    auto timestamp = ryu_timestamp_t{0};
    auto timestamp_ns = ryu_timestamp_ns_t{1};
    auto timestamp_ms = ryu_timestamp_ms_t{2};
    auto timestamp_sec = ryu_timestamp_sec_t{3};
    auto timestamp_tz = ryu_timestamp_tz_t{4};
    ASSERT_EQ(ryu_prepared_statement_bind_timestamp(&preparedStatement, "1", timestamp),
        RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_timestamp_ns(&preparedStatement, "2", timestamp_ns),
        RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_timestamp_ms(&preparedStatement, "3", timestamp_ms),
        RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_timestamp_sec(&preparedStatement, "4", timestamp_sec),
        RyuSuccess);
    ASSERT_EQ(ryu_prepared_statement_bind_timestamp_tz(&preparedStatement, "5", timestamp_tz),
        RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 7);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindInteval) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.lastJobDuration > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_prepared_statement_is_success(&preparedStatement));
    auto interval = ryu_interval_t{0, 0, 0};
    ASSERT_EQ(ryu_prepared_statement_bind_interval(&preparedStatement, "1", interval), RyuSuccess);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 8);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiPreparedStatementTest, BindValue) {
    ryu_prepared_statement preparedStatement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.registerTime > $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_prepared_statement_is_success(&preparedStatement));
    auto timestamp = ryu_timestamp_t{0};
    auto timestampValue = ryu_value_create_timestamp(timestamp);
    ASSERT_EQ(ryu_prepared_statement_bind_value(&preparedStatement, "1", timestampValue),
        RyuSuccess);
    ryu_value_destroy(timestampValue);
    state = ryu_connection_execute(connection, &preparedStatement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    ASSERT_EQ(ryu_query_result_get_num_tuples(&result), 1);
    ASSERT_EQ(ryu_query_result_get_num_columns(&result), 1);
    ASSERT_TRUE(ryu_query_result_is_success(&result));
    ASSERT_TRUE(ryu_query_result_has_next(&result));
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    auto tuple = resultCpp->getNext();
    auto value = tuple->getValue(0)->getValue<int64_t>();
    ASSERT_EQ(value, 7);
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}
