#pragma once

#include "c_api/ryu.h"
#include "graph_test/base_graph_test.h"

namespace ryu {
namespace testing {

// This class starts database in on-disk mode.
class APIDBTest : public BaseGraphTest {
public:
    void SetUp() override {
        BaseGraphTest::SetUp();
        createDBAndConn();
        initGraph();
    }
};

class CApiTest : public APIDBTest {
public:
    ryu_database _database;
    ryu_connection connection;

    void SetUp() override {
        APIDBTest::SetUp();
        auto* connCppPointer = conn.release();
        auto* databaseCppPointer = database.release();
        connection = ryu_connection{connCppPointer};
        _database = ryu_database{databaseCppPointer};
    }

    std::string getDatabasePath() { return databasePath; }

    ryu_database* getDatabase() { return &_database; }

    ryu_connection* getConnection() { return &connection; }

    void TearDown() override {
        ryu_connection_destroy(&connection);
        ryu_database_destroy(&_database);
        APIDBTest::TearDown();
    }
};

} // namespace testing
} // namespace ryu
