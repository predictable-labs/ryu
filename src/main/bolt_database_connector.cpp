#include "main/bolt_database_connector.h"

#include <cstring>
#include <regex>
#include <sstream>

extern "C" {
#include <neo4j-client.h>
}

#include "common/exception/exception.h"
#include "extension/extension_manager.h"
#include "main/database.h"

namespace ryu {
namespace main {

// Test-only flag to track if BoltDatabaseConnector was initialized
bool g_bolt_connector_test_initialized = false;

// Static initialization flag for neo4j client library
static bool neo4j_lib_initialized = false;

// Parse Bolt URL format: ryu://[username:password@]host:port/database
// or ryus://[username:password@]host:port/database (with TLS)
BoltConnectionInfo BoltConnectionInfo::parseURL(const std::string& url) {
    BoltConnectionInfo info;

    // Determine if TLS should be used
    info.useTLS = (url.rfind("ryus://", 0) == 0);

    // Remove the protocol prefix
    std::string urlWithoutProtocol;
    if (info.useTLS) {
        urlWithoutProtocol = url.substr(7); // Remove "ryus://"
    } else {
        urlWithoutProtocol = url.substr(6); // Remove "ryu://"
    }

    // Regular expression to parse the URL
    // Format: [username:password@]host:port/database
    std::regex urlRegex(R"((?:([^:]+):([^@]+)@)?([^:]+):(\d+)/(.+))");
    std::smatch matches;

    if (!std::regex_match(urlWithoutProtocol, matches, urlRegex)) {
        throw common::Exception(
            "Invalid Bolt URL format. Expected: ryu://[username:password@]host:port/database");
    }

    // Extract components
    info.username = matches[1].str();
    info.password = matches[2].str();
    info.host = matches[3].str();
    info.port = static_cast<uint16_t>(std::stoi(matches[4].str()));
    info.database = matches[5].str();

    return info;
}

BoltDatabaseConnector::BoltDatabaseConnector(std::string_view url, const SystemConfig& systemConfig)
    : connection(nullptr), config(nullptr), isConnected(false) {
    // Initialize neo4j client library once
    if (!neo4j_lib_initialized) {
        if (neo4j_client_init() != 0) {
            throw common::Exception("Failed to initialize neo4j client library");
        }
        neo4j_lib_initialized = true;
    }

    // Parse the connection URL
    connectionInfo = BoltConnectionInfo::parseURL(std::string(url));

    // Create neo4j config
    config = neo4j_new_config();
    if (config == nullptr) {
        throw common::Exception("Failed to create neo4j client configuration");
    }

    // Set authentication credentials if provided
    if (!connectionInfo.username.empty()) {
        if (neo4j_config_set_username(config, connectionInfo.username.c_str()) != 0) {
            neo4j_config_free(config);
            throw common::Exception("Failed to set username in neo4j config");
        }
    }

    if (!connectionInfo.password.empty()) {
        if (neo4j_config_set_password(config, connectionInfo.password.c_str()) != 0) {
            neo4j_config_free(config);
            throw common::Exception("Failed to set password in neo4j config");
        }
    }
}

BoltDatabaseConnector::~BoltDatabaseConnector() {
    disconnect();
    if (config != nullptr) {
        neo4j_config_free(config);
        config = nullptr;
    }
}

void BoltDatabaseConnector::connect() {
    if (isConnected) {
        return; // Already connected
    }

    // Build the connection URI for neo4j_connect
    // Format: neo4j://host:port or neo4js://host:port for TLS
    std::stringstream connectionUri;
    if (connectionInfo.useTLS) {
        connectionUri << "neo4j+s://";
    } else {
        connectionUri << "neo4j://";
    }

    connectionUri << connectionInfo.host << ":" << connectionInfo.port;

    // Connect to the Neo4j server using libneo4j-omni
    connection = neo4j_connect(connectionUri.str().c_str(), config, NEO4J_INSECURE);

    if (connection == nullptr) {
        std::stringstream ss;
        ss << "Failed to connect to Bolt server at " << connectionInfo.host << ":"
           << connectionInfo.port << " - " << strerror(errno);
        throw common::Exception(ss.str());
    }

    // Check if credentials expired
    if (neo4j_credentials_expired(connection)) {
        neo4j_close(connection);
        connection = nullptr;
        throw common::Exception("Neo4j credentials have expired");
    }

    isConnected = true;
}

void BoltDatabaseConnector::disconnect() {
    if (connection != nullptr) {
        neo4j_close(connection);
        connection = nullptr;
        isConnected = false;
    }
}

void BoltDatabaseConnector::initialize(Database* database) {
    // Set test flag
    g_bolt_connector_test_initialized = true;

    // Store the URL as the database path
    std::stringstream dbPath;
    dbPath << connectionInfo.host << ":" << connectionInfo.port;
    if (!connectionInfo.database.empty()) {
        dbPath << "/" << connectionInfo.database;
    }
    database->databasePath = dbPath.str();

    // Connect to the Bolt server (authentication is handled by libneo4j-omni during connection)
    connect();

    // Initialize minimal components for remote databases
    // Extension manager for potential client-side extensions
    database->extensionManager = std::make_unique<extension::ExtensionManager>();
    database->dbLifeCycleManager = std::make_shared<common::DatabaseLifeCycleManager>();

    // Note: For remote connections, we don't initialize local storage components
    // like BufferManager, StorageManager, etc. Instead, all operations will be
    // forwarded to the remote server via the Bolt protocol.
}

void BoltDatabaseConnector::cleanup(Database* database) {
    // Close the Bolt connection
    disconnect();

    if (database->dbLifeCycleManager) {
        database->dbLifeCycleManager->isDatabaseClosed = true;
    }
}

} // namespace main
} // namespace ryu
