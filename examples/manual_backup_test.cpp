#include "main/ryu.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>

using namespace ryu::main;
using namespace ryu::storage;

/**
 * Manual backup test program for Ryu database.
 *
 * This program demonstrates:
 * 1. Creating a database and populating it with data
 * 2. Starting a backup in the background
 * 3. Optionally performing concurrent writes during backup
 * 4. Monitoring backup progress
 * 5. Verifying the backup completed successfully
 * 6. Restoring from backup to a new database
 */

void printProgress(const char* label, double progress) {
    int barWidth = 50;
    std::cout << label << " [";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "%\r";
    std::cout.flush();
}

void createSampleData(Database& db) {
    std::cout << "\n=== Creating sample data ===" << std::endl;

    auto conn = std::make_unique<Connection>(&db);

    // Create a simple graph schema
    conn->query("CREATE NODE TABLE Person(name STRING, age INT64, PRIMARY KEY(name))");
    conn->query("CREATE NODE TABLE City(name STRING, population INT64, PRIMARY KEY(name))");
    conn->query("CREATE REL TABLE LivesIn(FROM Person TO City)");

    // Insert sample data
    std::cout << "Inserting people..." << std::endl;
    conn->query("CREATE (:Person {name: 'Alice', age: 30})");
    conn->query("CREATE (:Person {name: 'Bob', age: 25})");
    conn->query("CREATE (:Person {name: 'Carol', age: 35})");
    conn->query("CREATE (:Person {name: 'David', age: 28})");
    conn->query("CREATE (:Person {name: 'Eve', age: 32})");

    std::cout << "Inserting cities..." << std::endl;
    conn->query("CREATE (:City {name: 'New York', population: 8000000})");
    conn->query("CREATE (:City {name: 'London', population: 9000000})");
    conn->query("CREATE (:City {name: 'Tokyo', population: 14000000})");

    std::cout << "Creating relationships..." << std::endl;
    conn->query("MATCH (p:Person {name: 'Alice'}), (c:City {name: 'New York'}) CREATE (p)-[:LivesIn]->(c)");
    conn->query("MATCH (p:Person {name: 'Bob'}), (c:City {name: 'London'}) CREATE (p)-[:LivesIn]->(c)");
    conn->query("MATCH (p:Person {name: 'Carol'}), (c:City {name: 'Tokyo'}) CREATE (p)-[:LivesIn]->(c)");

    std::cout << "Sample data created successfully!" << std::endl;
}

void queryData(Database& db, const std::string& label) {
    std::cout << "\n=== Querying data (" << label << ") ===" << std::endl;

    auto conn = std::make_unique<Connection>(&db);

    // Query people
    auto result = conn->query("MATCH (p:Person) RETURN p.name, p.age ORDER BY p.name");
    std::cout << "People in database:" << std::endl;
    while (result->hasNext()) {
        auto row = result->getNext();
        std::cout << "  - " << row->getValue(0)->toString()
                  << " (age: " << row->getValue(1)->toString() << ")" << std::endl;
    }

    // Query cities
    result = conn->query("MATCH (c:City) RETURN c.name, c.population ORDER BY c.name");
    std::cout << "Cities in database:" << std::endl;
    while (result->hasNext()) {
        auto row = result->getNext();
        std::cout << "  - " << row->getValue(0)->toString()
                  << " (population: " << row->getValue(1)->toString() << ")" << std::endl;
    }
}

void testBasicBackup() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test 1: Basic Backup (No Concurrent Writes)" << std::endl;
    std::cout << "========================================" << std::endl;

    const std::string dbPath = "/tmp/ryu_backup_test_db";
    const std::string backupPath = "/tmp/ryu_backup_test_backup";
    const std::string restorePath = "/tmp/ryu_backup_test_restore";

    // Clean up previous test artifacts
    std::filesystem::remove_all(dbPath);
    std::filesystem::remove_all(backupPath);
    std::filesystem::remove_all(restorePath);

    // Step 1: Create and populate database
    {
        std::cout << "\nStep 1: Creating and populating database at " << dbPath << std::endl;
        auto db = std::make_unique<Database>(dbPath);
        createSampleData(*db);
        queryData(*db, "Original Database");

        // Step 2: Start backup
        std::cout << "\nStep 2: Starting backup to " << backupPath << std::endl;
        auto storageManager = db->getStorageManager();
        storageManager->startBackup(backupPath);

        // Step 3: Monitor backup progress
        std::cout << "\nStep 3: Monitoring backup progress..." << std::endl;
        BackupState state;
        do {
            state = storageManager->getBackupState();
            double progress = storageManager->getBackupProgress();
            printProgress("Backup progress", progress);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } while (state == BackupState::IN_PROGRESS);

        std::cout << std::endl;

        // Step 4: Check backup result
        if (state == BackupState::COMPLETED) {
            std::cout << "\n✓ Backup completed successfully!" << std::endl;
        } else if (state == BackupState::FAILED) {
            std::cout << "\n✗ Backup failed!" << std::endl;
            return;
        }

        // Database will close when db goes out of scope
    }

    // Step 5: Restore from backup
    std::cout << "\nStep 5: Restoring from backup to " << restorePath << std::endl;
    // Note: Restore functionality needs to be implemented
    // For now, we can manually copy the backup directory
    std::filesystem::copy(backupPath, restorePath,
                         std::filesystem::copy_options::recursive);

    // Step 6: Verify restored database
    std::cout << "\nStep 6: Verifying restored database..." << std::endl;
    {
        auto restoredDb = std::make_unique<Database>(restorePath);
        queryData(*restoredDb, "Restored Database");
    }

    std::cout << "\n✓ Test completed successfully!" << std::endl;
    std::cout << "\nTest artifacts:" << std::endl;
    std::cout << "  Original DB: " << dbPath << std::endl;
    std::cout << "  Backup:      " << backupPath << std::endl;
    std::cout << "  Restored DB: " << restorePath << std::endl;
}

void testConcurrentWritesBackup() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test 2: Backup with Concurrent Writes" << std::endl;
    std::cout << "========================================" << std::endl;

    const std::string dbPath = "/tmp/ryu_backup_concurrent_db";
    const std::string backupPath = "/tmp/ryu_backup_concurrent_backup";

    // Clean up previous test artifacts
    std::filesystem::remove_all(dbPath);
    std::filesystem::remove_all(backupPath);

    std::cout << "\nCreating and populating database..." << std::endl;
    auto db = std::make_unique<Database>(dbPath);
    createSampleData(*db);

    // Start backup
    std::cout << "\nStarting backup..." << std::endl;
    auto storageManager = db->getStorageManager();
    storageManager->startBackup(backupPath);

    // Perform concurrent writes
    std::cout << "\nPerforming concurrent writes during backup..." << std::endl;
    auto conn = std::make_unique<Connection>(db.get());

    // Add more data while backup is running
    conn->query("CREATE (:Person {name: 'Frank', age: 40})");
    conn->query("CREATE (:Person {name: 'Grace', age: 29})");
    conn->query("CREATE (:City {name: 'Paris', population: 2000000})");

    std::cout << "Concurrent writes completed" << std::endl;

    // Monitor backup progress
    std::cout << "\nWaiting for backup to complete..." << std::endl;
    BackupState state;
    do {
        state = storageManager->getBackupState();
        double progress = storageManager->getBackupProgress();
        printProgress("Backup progress", progress);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (state == BackupState::IN_PROGRESS);

    std::cout << std::endl;

    if (state == BackupState::COMPLETED) {
        std::cout << "\n✓ Backup with concurrent writes completed successfully!" << std::endl;
    } else {
        std::cout << "\n✗ Backup failed!" << std::endl;
    }

    // Show final database state
    queryData(*db, "Database After Concurrent Writes");

    std::cout << "\nTest artifacts:" << std::endl;
    std::cout << "  Database: " << dbPath << std::endl;
    std::cout << "  Backup:   " << backupPath << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Ryu Database Backup Manual Test" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // Test 1: Basic backup without concurrent writes
        testBasicBackup();

        std::cout << "\n\n";

        // Test 2: Backup with concurrent writes
        testConcurrentWritesBackup();

        std::cout << "\n========================================" << std::endl;
        std::cout << "All tests completed!" << std::endl;
        std::cout << "========================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
