#include "main/ryu.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

using namespace ryu::main;
using namespace ryu::storage;

/**
 * Simple backup test to demonstrate the actual backup functionality
 */

void printProgress(double progress) {
    int barWidth = 50;
    std::cout << "\rProgress [";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% ";
    std::cout.flush();
}

int main() {
    const std::string dbPath = "/tmp/ryu_test_db";
    const std::string backupPath = "/tmp/ryu_test_backup";

    // Clean up
    std::filesystem::remove_all(dbPath);
    std::filesystem::remove_all(backupPath);

    std::cout << "=== Ryu Database Backup Test ===" << std::endl;
    std::cout << "\n1. Creating database and adding data..." << std::endl;

    try {
        // Create database
        auto db = std::make_unique<Database>(dbPath);
        auto conn = std::make_unique<Connection>(db.get());

        // Create schema
        conn->query("CREATE NODE TABLE Person(name STRING, age INT64, PRIMARY KEY(name))");
        conn->query("CREATE NODE TABLE City(name STRING, PRIMARY KEY(name))");

        // Insert data
        conn->query("CREATE (:Person {name: 'Alice', age: 30})");
        conn->query("CREATE (:Person {name: 'Bob', age: 25})");
        conn->query("CREATE (:Person {name: 'Carol', age: 35})");
        conn->query("CREATE (:City {name: 'New York'})");
        conn->query("CREATE (:City {name: 'London'})");

        std::cout << "   ✓ Data created successfully\n" << std::endl;

        // Query to verify
        std::cout << "2. Querying original database:" << std::endl;
        auto result = conn->query("MATCH (p:Person) RETURN p.name, p.age ORDER BY p.name");
        while (result->hasNext()) {
            auto row = result->getNext();
            std::cout << "   - " << row->getValue(0)->toString()
                      << " (age: " << row->getValue(1)->toString() << ")" << std::endl;
        }

        // Start backup
        std::cout << "\n3. Starting backup to " << backupPath << "..." << std::endl;
        auto storageManager = db->getStorageManager();
        storageManager->startBackup(backupPath);

        // Monitor progress
        BackupState state;
        do {
            state = storageManager->getBackupState();
            double progress = storageManager->getBackupProgress();
            printProgress(progress);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } while (state == BackupState::IN_PROGRESS);

        std::cout << std::endl;

        // Check result
        if (state == BackupState::COMPLETED) {
            std::cout << "   ✓ Backup completed successfully!\n" << std::endl;
        } else {
            std::cout << "   ✗ Backup failed!\n" << std::endl;
            return 1;
        }

        // Show backup contents
        std::cout << "4. Backup directory contents:" << std::endl;
        for (const auto& entry : std::filesystem::directory_iterator(backupPath)) {
            auto size = std::filesystem::file_size(entry.path());
            std::cout << "   - " << entry.path().filename().string()
                      << " (" << size << " bytes)" << std::endl;
        }

        std::cout << "\n5. Reading backup metadata:" << std::endl;
        std::string metadataPath = backupPath + "/backup_metadata.json";
        if (std::filesystem::exists(metadataPath)) {
            std::ifstream metadataFile(metadataPath);
            std::string line;
            while (std::getline(metadataFile, line)) {
                std::cout << "   " << line << std::endl;
            }
        }

        std::cout << "\n=== Test Completed Successfully! ===" << std::endl;
        std::cout << "\nYou can find:" << std::endl;
        std::cout << "  Original DB: " << dbPath << std::endl;
        std::cout << "  Backup:      " << backupPath << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
