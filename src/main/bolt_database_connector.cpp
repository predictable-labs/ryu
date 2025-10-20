#include "main/bolt_database_connector.h"

#include <cstring>
#include <regex>
#include <sstream>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/exception/exception.h"
#include "extension/extension_manager.h"
#include "main/database.h"

namespace ryu {
namespace main {

// Test-only flag to track if BoltDatabaseConnector was initialized
bool g_bolt_connector_test_initialized = false;

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

BoltDatabaseConnector::BoltDatabaseConnector(std::string_view url, const SystemConfig& config)
    : socketFd(-1), isConnected(false) {
    // Note: config parameter is accepted for consistency with factory interface but not stored
    connectionInfo = BoltConnectionInfo::parseURL(std::string(url));
}

void BoltDatabaseConnector::connect() {
    // Create socket
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        throw common::Exception("Failed to create socket for Bolt connection");
    }

    // Resolve hostname
    struct hostent* server = gethostbyname(connectionInfo.host.c_str());
    if (server == nullptr) {
        close(socketFd);
        throw common::Exception("Failed to resolve hostname: " + connectionInfo.host);
    }

    // Setup server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    serverAddr.sin_port = htons(connectionInfo.port);

    // Connect to server
    if (::connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(socketFd);
        std::stringstream ss;
        ss << "Failed to connect to Bolt server at " << connectionInfo.host << ":"
           << connectionInfo.port;
        throw common::Exception(ss.str());
    }

    isConnected = true;

    // TODO: Perform Bolt handshake
    // - Send Bolt magic preamble (0x6060B017)
    // - Send supported protocol versions
    // - Receive agreed version from server
}

void BoltDatabaseConnector::authenticate() {
    if (!isConnected) {
        throw common::Exception("Cannot authenticate: not connected to Bolt server");
    }

    // TODO: Implement Bolt authentication
    // - Send HELLO message with authentication credentials
    // - Receive SUCCESS or FAILURE response
    // - Handle different authentication schemes (BASIC, KERBEROS, etc.)
}

void BoltDatabaseConnector::selectDatabase() {
    // TODO: Implement database selection
    // - In Bolt v4+, database selection is part of the HELLO message
    // - For earlier versions, might need to send a USE statement
}

void BoltDatabaseConnector::initialize(Database* database) {
    // Set test flag
    g_bolt_connector_test_initialized = true;

    // Store the URL as the database path
    database->databasePath = std::string(connectionInfo.host) + ":" +
        std::to_string(connectionInfo.port) + "/" + connectionInfo.database;

    // Connect to the Bolt server
    connect();

    // Authenticate if credentials provided
    if (!connectionInfo.username.empty()) {
        authenticate();
    }

    // Select the database
    if (!connectionInfo.database.empty()) {
        selectDatabase();
    }

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
    if (isConnected && socketFd >= 0) {
        close(socketFd);
        socketFd = -1;
        isConnected = false;
    }

    if (database->dbLifeCycleManager) {
        database->dbLifeCycleManager->isDatabaseClosed = true;
    }
}

} // namespace main
} // namespace ryu
