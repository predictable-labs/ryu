#include <filesystem>
#include <fstream>

#include "c_api/ryu.h"
#include "c_api_test/c_api_test.h"
#include "gtest/gtest.h"

using namespace ryu::main;
using namespace ryu::testing;
using namespace ryu::common;

class CApiVersionTest : public CApiTest {
public:
    std::string getInputDir() override { return TestHelper::appendRyuRootPath("dataset/tinysnb/"); }

    void TearDown() override { APIDBTest::TearDown(); }
};

class EmptyCApiVersionTest : public CApiVersionTest {
public:
    std::string getInputDir() override { return "empty"; }
};

TEST_F(EmptyCApiVersionTest, GetVersion) {
    ryu_connection_destroy(&connection);
    ryu_database_destroy(&_database);
    auto version = ryu_get_version();
    ASSERT_NE(version, nullptr);
    ASSERT_STREQ(version, RYU_CMAKE_VERSION);
    ryu_destroy_string(version);
}

TEST_F(CApiVersionTest, GetStorageVersion) {
    auto storageVersion = ryu_get_storage_version();
    if (inMemMode) {
        GTEST_SKIP();
    }
    // Reset the database to ensure that the lock on db file is released.
    ryu_connection_destroy(&connection);
    ryu_database_destroy(&_database);
    auto data = std::filesystem::path(databasePath);
    std::ifstream dbFile;
    dbFile.open(data, std::ios::binary);
    ASSERT_TRUE(dbFile.is_open());
    char magic[4];
    dbFile.read(magic, 3);
    magic[3] = '\0';
    ASSERT_STREQ(magic, "RYU");
    uint64_t actualVersion;
    dbFile.read(reinterpret_cast<char*>(&actualVersion), sizeof(actualVersion));
    dbFile.close();
    ASSERT_EQ(storageVersion, actualVersion);
}

TEST_F(EmptyCApiVersionTest, GetStorageVersion) {
    auto storageVersion = ryu_get_storage_version();
    if (inMemMode) {
        GTEST_SKIP();
    }
    // Reset the database to ensure that the lock on db file is released.
    ryu_connection_destroy(&connection);
    ryu_database_destroy(&_database);
    auto data = std::filesystem::path(databasePath);
    std::ifstream dbFile;
    dbFile.open(data, std::ios::binary);
    ASSERT_TRUE(dbFile.is_open());
    char magic[4];
    dbFile.read(magic, 3);
    magic[3] = '\0';
    ASSERT_STREQ(magic, "RYU");
    uint64_t actualVersion;
    dbFile.read(reinterpret_cast<char*>(&actualVersion), sizeof(actualVersion));
    dbFile.close();
    ASSERT_EQ(storageVersion, actualVersion);
}
