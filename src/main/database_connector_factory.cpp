#include "main/database_connector_factory.h"

#include "main/bolt_database_connector.h"

namespace ryu {
namespace main {

DatabaseConnectionType DatabaseConnectorFactory::detectConnectionType(std::string_view path) {
    // Check for Bolt protocol URLs
    if (path.rfind("ryu://", 0) == 0 || path.rfind("ryus://", 0) == 0) {
        return DatabaseConnectionType::BOLT;
    }

    // Default to embedded (file-based or in-memory)
    return DatabaseConnectionType::EMBEDDED;
}

std::unique_ptr<DatabaseConnector> DatabaseConnectorFactory::createConnector(
    std::string_view databasePath, const SystemConfig& config) {

    auto connectionType = detectConnectionType(databasePath);

    switch (connectionType) {
    case DatabaseConnectionType::BOLT:
        return std::make_unique<BoltDatabaseConnector>(databasePath, config);
    case DatabaseConnectionType::EMBEDDED:
    default:
        // For embedded databases, we don't use a connector - Database::initMembers handles it
        // This should never be called for embedded databases
        return nullptr;
    }
}

} // namespace main
} // namespace ryu
