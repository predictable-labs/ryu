#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "common/api.h"
#include "main/db_config.h"
#include "ryu_fwd.h"

namespace ryu {
namespace catalog {
class Catalog;
}

namespace storage {
class StorageManager;
class BufferManager;
class MemoryManager;
}

namespace transaction {
class TransactionManager;
}

namespace processor {
class QueryProcessor;
}

namespace extension {
class ExtensionManager;
}

namespace common {
class VirtualFileSystem;
}

namespace main {

class Database;
class DatabaseManager;

enum class DatabaseConnectionType {
    EMBEDDED,  // Local file-based or in-memory
    BOLT       // Remote Bolt protocol connection
};

/**
 * @brief Abstract base class for database connectors.
 * Connectors handle the initialization and management of database components
 * for different connection types (embedded vs remote).
 */
class DatabaseConnector {
public:
    virtual ~DatabaseConnector() = default;

    /**
     * @brief Returns the connection type for this connector
     */
    virtual DatabaseConnectionType getConnectionType() const = 0;

    /**
     * @brief Initializes the database connection and components
     * @param database The database instance being initialized
     */
    virtual void initialize(Database* database) = 0;

    /**
     * @brief Cleanup resources before database destruction
     * @param database The database instance being cleaned up
     */
    virtual void cleanup(Database* database) = 0;

    /**
     * @brief Returns whether this connection is remote
     */
    bool isRemote() const {
        return getConnectionType() == DatabaseConnectionType::BOLT;
    }

    /**
     * @brief Returns whether this connection is embedded (local)
     */
    bool isEmbedded() const {
        return getConnectionType() == DatabaseConnectionType::EMBEDDED;
    }
};

} // namespace main
} // namespace ryu
