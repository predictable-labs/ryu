#include "c_api/ryu.h"
#include "graph_test/base_graph_test.h"
#include "gtest/gtest.h"
#include "main/database.h"

using namespace ryu::main;
using namespace ryu::testing;

// This class starts database without initializing graph.
class APIEmptyDBTest : public BaseGraphTest {
    std::string getInputDir() override { KU_UNREACHABLE; }
};

class RemoteDatabaseTest : public APIEmptyDBTest {
public:
    void SetUp() override {
        APIEmptyDBTest::SetUp();
        // Reset test state before each test
        resetBoltConnectorTestState();
    }

    void TearDown() override {
        APIEmptyDBTest::TearDown();
        resetBoltConnectorTestState();
    }

    // Helper to reset the test flag
    static void resetBoltConnectorTestState();

    // Helper to check if BoltDatabaseConnector was initialized
    static bool wasBoltConnectorInitialized();
};

// Test that embedded database paths don't trigger remote connector
TEST_F(RemoteDatabaseTest, EmbeddedFilePathDoesNotUseBoltConnector) {
    // Create embedded database with file path
    auto db = std::make_unique<Database>(databasePath, SystemConfig());

    // Should NOT use BoltDatabaseConnector
    ASSERT_FALSE(db->isRemoteDatabase());
    ASSERT_FALSE(wasBoltConnectorInitialized());
}

// Test that :memory: doesn't trigger remote connector
TEST_F(RemoteDatabaseTest, InMemoryDoesNotUseBoltConnector) {
    // Create in-memory database
    auto db = std::make_unique<Database>(":memory:", SystemConfig());

    // Should NOT use BoltDatabaseConnector
    ASSERT_FALSE(db->isRemoteDatabase());
    ASSERT_FALSE(wasBoltConnectorInitialized());
}

// Test that ryu:// URL is detected as remote
TEST_F(RemoteDatabaseTest, RyuProtocolURLDetectedAsRemote) {
    // This will fail to connect since there's no server, but we can check detection
    try {
        auto db = std::make_unique<Database>("ryu://localhost:7687/testdb", SystemConfig());

        // If we get here, connection succeeded (unlikely in test environment)
        ASSERT_TRUE(db->isRemoteDatabase());
        ASSERT_TRUE(wasBoltConnectorInitialized());
    } catch (const std::exception& e) {
        // Expected to fail connection, but we can still verify it tried
        ASSERT_TRUE(wasBoltConnectorInitialized());
    }
}

// Test that ryus:// URL is detected as remote with TLS
TEST_F(RemoteDatabaseTest, RyusProtocolURLDetectedAsRemoteWithTLS) {
    // This will fail to connect since there's no server, but we can check detection
    try {
        auto db = std::make_unique<Database>("ryus://localhost:9000/testdb", SystemConfig());

        // If we get here, connection succeeded (unlikely in test environment)
        ASSERT_TRUE(db->isRemoteDatabase());
        ASSERT_TRUE(wasBoltConnectorInitialized());
    } catch (const std::exception& e) {
        // Expected to fail connection, but we can still verify it tried
        ASSERT_TRUE(wasBoltConnectorInitialized());
    }
}

// Test URL with authentication
TEST_F(RemoteDatabaseTest, URLWithAuthenticationParsedCorrectly) {
    try {
        auto db = std::make_unique<Database>("ryu://user:pass@server:7687/mydb", SystemConfig());
        ASSERT_TRUE(db->isRemoteDatabase());
    } catch (const std::exception& e) {
        // Expected to fail connection
        ASSERT_TRUE(wasBoltConnectorInitialized());
    }
}

// Implementation of helper functions
// These will be defined in the test setup to access BoltDatabaseConnector internals

namespace ryu {
namespace main {
// Add a test-only static flag to BoltDatabaseConnector
extern bool g_bolt_connector_test_initialized;
} // namespace main
} // namespace ryu

bool ryu::main::g_bolt_connector_test_initialized = false;

void RemoteDatabaseTest::resetBoltConnectorTestState() {
    ryu::main::g_bolt_connector_test_initialized = false;
}

bool RemoteDatabaseTest::wasBoltConnectorInitialized() {
    return ryu::main::g_bolt_connector_test_initialized;
}
