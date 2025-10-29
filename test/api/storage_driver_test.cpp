#include "api_test/api_test.h"
#include "main/storage_driver.h"

using namespace ryu::main;
using namespace ryu::testing;
using namespace ryu::common;

// TODO(Guodong): FIX-ME. Rework storage driver scan.
// TEST_F(ApiTest, StorageDriverScan) {
//     auto storageDriver = std::make_unique<StorageDriver>(database.get());
//     auto size = 6;
//     auto nodeOffsetsBuffer = std::make_unique<uint8_t[]>(sizeof(offset_t) * size);
//     auto nodeOffsets = (offset_t*)nodeOffsetsBuffer.get();
//     nodeOffsets[0] = 7;
//     nodeOffsets[1] = 0;
//     nodeOffsets[2] = 3;
//     nodeOffsets[3] = 1;
//     nodeOffsets[4] = 2;
//     nodeOffsets[5] = 6;
//     auto result = std::make_unique<uint8_t[]>(sizeof(int64_t) * size);
//     auto resultBuffer = (uint8_t*)result.get();
//     storageDriver->scan("person", "ID", nodeOffsets, size, resultBuffer, 3);
//     auto ids = (int64_t*)resultBuffer;
//     ASSERT_EQ(ids[0], 10);
//     ASSERT_EQ(ids[1], 0);
//     ASSERT_EQ(ids[2], 5);
//     ASSERT_EQ(ids[3], 2);
//     ASSERT_EQ(ids[4], 3);
//     ASSERT_EQ(ids[5], 9);
// }

// Test for issue #6020: getNumRels should work with RelGroup entries
TEST_F(ApiTest, StorageDriverGetNumRels) {
    auto storageDriver = std::make_unique<StorageDriver>(database.get());

    // Test getting count for "knows" relationship from tinysnb dataset
    auto numKnows = storageDriver->getNumRels("knows");

    // Verify the count matches what we get from a Cypher query
    auto result = conn->query("MATCH ()-[:knows]->() RETURN COUNT(*)");
    ASSERT_TRUE(result->hasNext());
    auto tuple = result->getNext();
    auto cypherCount = tuple->getValue(0)->getValue<int64_t>();

    ASSERT_EQ(numKnows, static_cast<uint64_t>(cypherCount));
    ASSERT_FALSE(result->hasNext());
}

// Test for issue #6020: getNumRels with multiple relationship types
TEST_F(ApiTest, StorageDriverGetNumRelsMultipleTypes) {
    auto storageDriver = std::make_unique<StorageDriver>(database.get());

    // Test "knows" relationship
    auto numKnows = storageDriver->getNumRels("knows");
    auto resultKnows = conn->query("MATCH ()-[:knows]->() RETURN COUNT(*)");
    ASSERT_TRUE(resultKnows->hasNext());
    ASSERT_EQ(numKnows, static_cast<uint64_t>(resultKnows->getNext()->getValue(0)->getValue<int64_t>()));

    // Test "studyAt" relationship
    auto numStudyAt = storageDriver->getNumRels("studyAt");
    auto resultStudyAt = conn->query("MATCH ()-[:studyAt]->() RETURN COUNT(*)");
    ASSERT_TRUE(resultStudyAt->hasNext());
    ASSERT_EQ(numStudyAt, static_cast<uint64_t>(resultStudyAt->getNext()->getValue(0)->getValue<int64_t>()));

    // Test "workAt" relationship
    auto numWorkAt = storageDriver->getNumRels("workAt");
    auto resultWorkAt = conn->query("MATCH ()-[:workAt]->() RETURN COUNT(*)");
    ASSERT_TRUE(resultWorkAt->hasNext());
    ASSERT_EQ(numWorkAt, static_cast<uint64_t>(resultWorkAt->getNext()->getValue(0)->getValue<int64_t>()));
}

// Test for issue #6020: getNumRels with custom relationship
TEST_F(ApiTest, StorageDriverGetNumRelsCustom) {
    // Create a custom relationship to test
    conn->query("CREATE NODE TABLE TestNode(id INT64, PRIMARY KEY(id))");
    conn->query("CREATE REL TABLE TestRel(FROM TestNode TO TestNode)");

    // Insert test data
    conn->query("CREATE (n:TestNode {id: 1})");
    conn->query("CREATE (n:TestNode {id: 2})");
    conn->query("CREATE (n:TestNode {id: 3})");
    conn->query("MATCH (a:TestNode {id: 1}), (b:TestNode {id: 2}) CREATE (a)-[:TestRel]->(b)");
    conn->query("MATCH (a:TestNode {id: 2}), (b:TestNode {id: 3}) CREATE (a)-[:TestRel]->(b)");
    conn->query("MATCH (a:TestNode {id: 1}), (b:TestNode {id: 3}) CREATE (a)-[:TestRel]->(b)");

    auto storageDriver = std::make_unique<StorageDriver>(database.get());
    auto numTestRel = storageDriver->getNumRels("TestRel");

    // Verify count
    auto result = conn->query("MATCH ()-[:TestRel]->() RETURN COUNT(*)");
    ASSERT_TRUE(result->hasNext());
    auto cypherCount = result->getNext()->getValue(0)->getValue<int64_t>();

    ASSERT_EQ(numTestRel, static_cast<uint64_t>(cypherCount));
    ASSERT_EQ(numTestRel, 3);
}

// Test for issue #6020: getNumRels returns 0 for empty relationship
TEST_F(ApiTest, StorageDriverGetNumRelsEmpty) {
    // Create an empty relationship
    conn->query("CREATE NODE TABLE EmptyNodeA(id INT64, PRIMARY KEY(id))");
    conn->query("CREATE NODE TABLE EmptyNodeB(id INT64, PRIMARY KEY(id))");
    conn->query("CREATE REL TABLE EmptyRel(FROM EmptyNodeA TO EmptyNodeB)");

    auto storageDriver = std::make_unique<StorageDriver>(database.get());
    auto numEmptyRel = storageDriver->getNumRels("EmptyRel");

    ASSERT_EQ(numEmptyRel, 0);
}
