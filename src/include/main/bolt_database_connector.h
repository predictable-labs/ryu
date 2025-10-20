#pragma once

#include <memory>
#include <string>

#include "main/database_connector.h"

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

    DatabaseConnectionType getConnectionType() const override {
        return DatabaseConnectionType::BOLT;
    }

    void initialize(Database* database) override;
    void cleanup(Database* database) override;

    const BoltConnectionInfo& getConnectionInfo() const { return connectionInfo; }

private:
    void connect();
    void authenticate();
    void selectDatabase();

    BoltConnectionInfo connectionInfo;

    // Bolt protocol state
    int socketFd;
    bool isConnected;
};

} // namespace main
} // namespace ryu
