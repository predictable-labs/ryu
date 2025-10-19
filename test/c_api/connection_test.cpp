#include <thread>

#include "c_api_test/c_api_test.h"

using ::testing::Test;
using namespace ryu::main;
using namespace ryu::testing;

class CApiConnectionTest : public CApiTest {
public:
    std::string getInputDir() override { return TestHelper::appendRyuRootPath("dataset/tinysnb/"); }
};

TEST_F(CApiConnectionTest, CreationAndDestroy) {
    ryu_connection connection;
    ryu_state state;
    auto _database = getDatabase();
    state = ryu_connection_init(_database, &connection);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(connection._connection, nullptr);
    auto connectionCpp = static_cast<Connection*>(connection._connection);
    ASSERT_NE(connectionCpp, nullptr);
    ryu_connection_destroy(&connection);
}

TEST_F(CApiConnectionTest, CreationAndDestroyWithNullDatabase) {
    ryu_connection connection;
    ryu_state state;
    state = ryu_connection_init(nullptr, &connection);
    ASSERT_EQ(state, RyuError);
}

TEST_F(CApiConnectionTest, Query) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) RETURN a.fName";
    state = ryu_connection_query(connection, query, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    ASSERT_NE(resultCpp, nullptr);
    ASSERT_TRUE(resultCpp->isSuccess());
    ASSERT_TRUE(resultCpp->hasNext());
    ASSERT_EQ(resultCpp->getErrorMessage(), "");
    ASSERT_EQ(resultCpp->getNumTuples(), 8);
    ASSERT_EQ(resultCpp->getNumColumns(), 1);
    ASSERT_EQ(resultCpp->getColumnNames()[0], "a.fName");
    ryu_query_result_destroy(&result);
}

TEST_F(CApiConnectionTest, SetGetMaxNumThreadForExec) {
    uint64_t maxNumThreadForExec;
    ryu_state state;
    auto connection = getConnection();
    state = ryu_connection_set_max_num_thread_for_exec(connection, 4);
    ASSERT_EQ(state, RyuSuccess);
    auto connectionCpp = static_cast<Connection*>(connection->_connection);
    ASSERT_EQ(connectionCpp->getMaxNumThreadForExec(), 4);
    state = ryu_connection_get_max_num_thread_for_exec(connection, &maxNumThreadForExec);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(maxNumThreadForExec, 4);
    state = ryu_connection_set_max_num_thread_for_exec(connection, 8);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(connectionCpp->getMaxNumThreadForExec(), 8);
    state = ryu_connection_get_max_num_thread_for_exec(connection, &maxNumThreadForExec);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_EQ(maxNumThreadForExec, 8);
    ryu_connection badConnection;
    ASSERT_EQ(ryu_connection_init(nullptr, &badConnection), RyuError);
    state = ryu_connection_set_max_num_thread_for_exec(&badConnection, 4);
    ASSERT_EQ(state, RyuError);
    state = ryu_connection_get_max_num_thread_for_exec(&badConnection, &maxNumThreadForExec);
    ASSERT_EQ(state, RyuError);
}

TEST_F(CApiConnectionTest, Prepare) {
    ryu_prepared_statement statement;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &statement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(statement._prepared_statement, nullptr);
    ASSERT_NE(statement._bound_values, nullptr);
    auto statementCpp = static_cast<PreparedStatement*>(statement._prepared_statement);
    ASSERT_NE(statementCpp, nullptr);
    auto connectionCpp = static_cast<Connection*>(connection->_connection);
    auto result = connectionCpp->execute(statementCpp, std::make_pair(std::string("1"), true));
    ASSERT_TRUE(result->isSuccess());
    ASSERT_TRUE(result->hasNext());
    ASSERT_EQ(result->getErrorMessage(), "");
    ASSERT_EQ(result->getNumTuples(), 1);
    auto tuple = result->getNext();
    ASSERT_EQ(tuple->getValue(0)->getValue<int64_t>(), 3);
    ryu_prepared_statement_destroy(&statement);
}

TEST_F(CApiConnectionTest, Execute) {
    ryu_prepared_statement statement;
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &statement);
    ASSERT_EQ(state, RyuSuccess);
    ryu_prepared_statement_bind_bool(&statement, "1", true);
    state = ryu_connection_execute(connection, &statement, &result);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(result._query_result, nullptr);
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    ASSERT_NE(resultCpp, nullptr);
    ASSERT_TRUE(resultCpp->isSuccess());
    ASSERT_TRUE(resultCpp->hasNext());
    ASSERT_EQ(resultCpp->getErrorMessage(), "");
    ASSERT_EQ(resultCpp->getNumTuples(), 1);
    auto tuple = resultCpp->getNext();
    ASSERT_EQ(tuple->getValue(0)->getValue<int64_t>(), 3);
    ryu_prepared_statement_destroy(&statement);
    ryu_query_result_destroy(&result);
}

TEST_F(CApiConnectionTest, ExecuteError) {
    ryu_prepared_statement preparedStatement;
    ryu_state state;
    auto connection = getConnection();
    auto query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
    state = ryu_connection_prepare(connection, query, &preparedStatement);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(preparedStatement._prepared_statement, nullptr);
    ASSERT_EQ(ryu_prepared_statement_bind_int64(&preparedStatement, "1", 30), RyuSuccess);
    ryu_query_result result;
    ASSERT_EQ(ryu_connection_execute(connection, &preparedStatement, &result), RyuError);
    ASSERT_FALSE(ryu_query_result_is_success(&result));
    ryu_query_result_destroy(&result);
    ryu_prepared_statement_destroy(&preparedStatement);
}

TEST_F(CApiConnectionTest, QueryTimeout) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    ASSERT_EQ(ryu_connection_set_query_timeout(connection, 1), RyuSuccess);
    state = ryu_connection_query(connection,
        "UNWIND RANGE(1,100000) AS x UNWIND RANGE(1, 100000) AS y RETURN COUNT(x + y);", &result);
    ASSERT_EQ(state, RyuError);
    ASSERT_NE(result._query_result, nullptr);
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    ASSERT_NE(resultCpp, nullptr);
    ASSERT_FALSE(resultCpp->isSuccess());
    ASSERT_EQ(resultCpp->getErrorMessage(), "Interrupted.");
    ryu_query_result_destroy(&result);
    ryu_connection badConnection;
    ASSERT_EQ(ryu_connection_init(nullptr, &badConnection), RyuError);
    ASSERT_EQ(ryu_connection_set_query_timeout(&badConnection, 1), RyuError);
}

#ifndef __SINGLE_THREADED__
// The following test is disabled in single-threaded mode because it requires
// a separate thread to run.
TEST_F(CApiConnectionTest, Interrupt) {
    ryu_query_result result;
    ryu_state state;
    auto connection = getConnection();
    std::atomic<bool> finished = false;

    // Interrupt the query after 100ms
    // This may happen too early, so try again until the query function finishes.
    std::thread t([&connection, &finished]() {
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ryu_connection_interrupt(connection);
        } while (!finished);
    });
    state = ryu_connection_query(connection,
        "UNWIND RANGE(1,100000) AS x UNWIND RANGE(1, 100000) AS y RETURN COUNT(x + y);", &result);
    finished = true;
    ASSERT_EQ(state, RyuError);
    ASSERT_NE(result._query_result, nullptr);
    auto resultCpp = static_cast<QueryResult*>(result._query_result);
    ASSERT_NE(resultCpp, nullptr);
    ASSERT_FALSE(resultCpp->isSuccess());
    ASSERT_EQ(resultCpp->getErrorMessage(), "Interrupted.");
    ryu_query_result_destroy(&result);
    t.join();
}
#endif
