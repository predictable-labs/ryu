#pragma once

#include <memory>
#include <string>

#include "main/database_connector.h"

// Forward declare neo4j client types to avoid including C header in header
struct neo4j_connection;
struct neo4j_config;

namespace ryu {
namespace main {

struct BoltConnectionInfo {
    std::string host;
    uint16_t port;
    std::string database;
    std::string username;
    std::string password;
    bool useTLS;

    static BoltConnectionInfo parseURL(const std::string& url);
};

/**
 * @brief Connector for remote databases using the Bolt protocol.
 * Implements a Bolt wire protocol client to connect to existing Bolt servers.
 */
class BoltDatabaseConnector : public DatabaseConnector {
public:
    explicit BoltDatabaseConnector(std::string_view url, const SystemConfig& config);
    ~BoltDatabaseConnector();

    DatabaseConnectionType getConnectionType() const override {
        return DatabaseConnectionType::BOLT;
    }

    void initialize(Database* database) override;
    void cleanup(Database* database) override;

    const BoltConnectionInfo& getConnectionInfo() const { return connectionInfo; }

private:
    void connect();
    void disconnect();

    BoltConnectionInfo connectionInfo;

    // libneo4j-omni connection state
    neo4j_connection* connection;
    neo4j_config* config;
    bool isConnected;
};

} // namespace main
} // namespace ryu
