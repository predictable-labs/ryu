#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "main/database_connector.h"
#include "main/db_config.h"

namespace ryu {
namespace main {

/**
 * @brief Factory for creating appropriate database connectors based on the database path/URL.
 */
class DatabaseConnectorFactory {
public:
    /**
     * @brief Creates a database connector based on the provided path/URL.
     *
     * URL Format Detection:
     * - "ryu://host:port/database" or "ryus://host:port/database" → BoltDatabaseConnector
     * - ":memory:" or file paths → EmbeddedDatabaseConnector
     *
     * @param databasePath Database path or URL
     * @param config System configuration
     * @return Unique pointer to the appropriate connector
     */
    static std::unique_ptr<DatabaseConnector> createConnector(std::string_view databasePath,
        const SystemConfig& config);

private:
    static DatabaseConnectionType detectConnectionType(std::string_view path);
};

} // namespace main
} // namespace ryu
