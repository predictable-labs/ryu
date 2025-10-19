#include "c_api/ryu.h"
#include "graph_test/base_graph_test.h"
#include "gtest/gtest.h"

using namespace ryu::main;
using namespace ryu::testing;

// This class starts database without initializing graph.
class APIEmptyDBTest : public BaseGraphTest {
    std::string getInputDir() override { KU_UNREACHABLE; }
};

class CApiDatabaseTest : public APIEmptyDBTest {
public:
    void SetUp() override {
        APIEmptyDBTest::SetUp();
        defaultSystemConfig = ryu_default_system_config();

        // limit memory usage by keeping max number of threads small
        defaultSystemConfig.max_num_threads = 2;
        auto maxDBSizeEnv = TestHelper::getSystemEnv("MAX_DB_SIZE");
        if (!maxDBSizeEnv.empty()) {
            defaultSystemConfig.max_db_size = std::stoull(maxDBSizeEnv);
        }
    }

    ryu_system_config defaultSystemConfig;
};

TEST_F(CApiDatabaseTest, CreationAndDestroy) {
    ryu_database database;
    ryu_state state;
    auto databasePathCStr = databasePath.c_str();
    auto systemConfig = defaultSystemConfig;
    state = ryu_database_init(databasePathCStr, systemConfig, &database);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(database._database, nullptr);
    auto databaseCpp = static_cast<Database*>(database._database);
    ASSERT_NE(databaseCpp, nullptr);
    ryu_database_destroy(&database);
}

TEST_F(CApiDatabaseTest, CreationReadOnly) {
    ryu_database database;
    ryu_connection connection;
    ryu_query_result queryResult;
    ryu_state state;
    auto databasePathCStr = databasePath.c_str();
    auto systemConfig = defaultSystemConfig;
    // First, create a read-write database.
    state = ryu_database_init(databasePathCStr, systemConfig, &database);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(database._database, nullptr);
    auto databaseCpp = static_cast<Database*>(database._database);
    ASSERT_NE(databaseCpp, nullptr);
    ryu_database_destroy(&database);
    // Now, access the same database read-only.
    systemConfig.read_only = true;
    state = ryu_database_init(databasePathCStr, systemConfig, &database);
    if (databasePath == "" || databasePath == ":memory:") {
        ASSERT_EQ(state, RyuError);
        ASSERT_EQ(database._database, nullptr);
        return;
    }
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(database._database, nullptr);
    databaseCpp = static_cast<Database*>(database._database);
    ASSERT_NE(databaseCpp, nullptr);
    // Try to write to the database.
    state = ryu_connection_init(&database, &connection);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_connection_query(&connection,
        "CREATE NODE TABLE User(name STRING, age INT64, reg_date DATE, PRIMARY KEY (name))",
        &queryResult);
    ASSERT_EQ(state, RyuError);
    ASSERT_FALSE(ryu_query_result_is_success(&queryResult));
    ryu_query_result_destroy(&queryResult);
    ryu_connection_destroy(&connection);
    ryu_database_destroy(&database);
}

TEST_F(CApiDatabaseTest, CreationInMemory) {
    ryu_database database;
    ryu_state state;
    auto databasePathCStr = (char*)"";
    state = ryu_database_init(databasePathCStr, defaultSystemConfig, &database);
    ASSERT_EQ(state, RyuSuccess);
    ryu_database_destroy(&database);
    databasePathCStr = (char*)":memory:";
    state = ryu_database_init(databasePathCStr, defaultSystemConfig, &database);
    ASSERT_EQ(state, RyuSuccess);
    ryu_database_destroy(&database);
}

#ifndef __WASM__ // home directory is not available in WASM
TEST_F(CApiDatabaseTest, CreationHomeDir) {
    ryu_database database;
    ryu_connection connection;
    ryu_state state;
    auto databasePathCStr = (char*)"~/ku_test.db";
    state = ryu_database_init(databasePathCStr, defaultSystemConfig, &database);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_connection_init(&database, &connection);
    ASSERT_EQ(state, RyuSuccess);
    auto homePath =
        getClientContext(*(Connection*)(connection._connection))->getClientConfig()->homeDirectory;
    ryu_connection_destroy(&connection);
    ryu_database_destroy(&database);
    std::filesystem::remove_all(homePath + "/ku_test.db");
}
#endif

TEST_F(CApiDatabaseTest, CloseQueryResultAndConnectionAfterDatabaseDestroy) {
    ryu_database database;
    auto databasePathCStr = (char*)":memory:";
    auto systemConfig = ryu_default_system_config();
    systemConfig.buffer_pool_size = 10 * 1024 * 1024; // 10MB
    systemConfig.max_db_size = 1 << 30;               // 1GB
    systemConfig.max_num_threads = 2;
    ryu_state state = ryu_database_init(databasePathCStr, systemConfig, &database);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_NE(database._database, nullptr);
    ryu_connection conn;
    ryu_query_result queryResult;
    state = ryu_connection_init(&database, &conn);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_connection_query(&conn, "RETURN 1+1", &queryResult);
    ASSERT_EQ(state, RyuSuccess);
    ASSERT_TRUE(ryu_query_result_is_success(&queryResult));
    ryu_flat_tuple tuple;
    ryu_state resultState = ryu_query_result_get_next(&queryResult, &tuple);
    ASSERT_EQ(resultState, RyuSuccess);
    ryu_value value;
    ryu_state valueState = ryu_flat_tuple_get_value(&tuple, 0, &value);
    ASSERT_EQ(valueState, RyuSuccess);
    int64_t valueInt = INT64_MAX;
    ryu_state valueIntState = ryu_value_get_int64(&value, &valueInt);
    ASSERT_EQ(valueIntState, RyuSuccess);
    ASSERT_EQ(valueInt, 2);
    // Destroy database first, this should not crash
    ryu_database_destroy(&database);
    // Call ryu_connection_query should not crash, but return an error
    state = ryu_connection_query(&conn, "RETURN 1+1", &queryResult);
    ASSERT_EQ(state, RyuError);
    // Call ryu_query_result_get_next should not crash, but return an error
    resultState = ryu_query_result_get_next(&queryResult, &tuple);
    ASSERT_EQ(resultState, RyuError);
    // Now destroy everything, this should not crash
    ryu_query_result_destroy(&queryResult);
    ryu_connection_destroy(&conn);
    ryu_value_destroy(&value);
    ryu_flat_tuple_destroy(&tuple);
}

TEST_F(CApiDatabaseTest, UseConnectionAfterDatabaseDestroy) {
    ryu_database db;
    ryu_connection conn;
    ryu_query_result result;

    auto systemConfig = ryu_default_system_config();
    systemConfig.buffer_pool_size = 10 * 1024 * 1024; // 10MB
    systemConfig.max_db_size = 1 << 30;               // 1GB
    systemConfig.max_num_threads = 2;
    auto state = ryu_database_init("", systemConfig, &db);
    ASSERT_EQ(state, RyuSuccess);
    state = ryu_connection_init(&db, &conn);
    ASSERT_EQ(state, RyuSuccess);
    ryu_database_destroy(&db);
    state = ryu_connection_query(&conn, "RETURN 0", &result);
    ASSERT_EQ(state, RyuError);

    ryu_connection_destroy(&conn);
}
