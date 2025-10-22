# Backup Testing Guide

This guide explains how to manually test the backup and restore functionality in Ryu.

## Quick Start

### Option 1: Using the Shell Script (Recommended for Quick Testing)

```bash
# Build the project first
make release

# Run the full backup test
./test_backup.sh test

# Or run individual commands:
./test_backup.sh create   # Create test database
./test_backup.sh query    # Query the database
./test_backup.sh backup   # Create a backup
./test_backup.sh restore  # Restore from backup
./test_backup.sh inspect  # Inspect database files
./test_backup.sh cleanup  # Clean up test files
```

### Option 2: Using the C++ Test Program (For Comprehensive Testing)

First, you need to build the manual test program:

```bash
# Build the test program
cd build/release
make manual_backup_test  # (Note: You'll need to add this target to CMakeLists.txt)

# Or compile manually:
g++ -std=c++20 -I../../src/include \
    ../../examples/manual_backup_test.cpp \
    -L. -lryu \
    -o manual_backup_test

# Run the test
./manual_backup_test
```

## Understanding the Backup API

### Key Classes

1. **BackupManager** ([src/include/storage/backup/backup_manager.h](src/include/storage/backup/backup_manager.h))
   - Main class for backup operations
   - Methods:
     - `startBackup(path)` - Start async backup
     - `waitForCompletion()` - Wait for backup to finish
     - `getBackupState()` - Get current state
     - `getBackupProgress()` - Get progress (0.0 to 1.0)
     - `cancelBackup()` - Cancel ongoing backup
     - `restoreFromBackup(backupPath, targetPath)` - Static restore method

2. **StorageManager** ([src/include/storage/storage_manager.h](src/include/storage/storage_manager.h))
   - Exposes backup API:
     - `startBackup(path)`
     - `getBackupState()`
     - `getBackupProgress()`
     - `waitForBackupCompletion()`

3. **BackupState** Enum:
   - `IDLE` - No backup in progress
   - `IN_PROGRESS` - Backup currently running
   - `COMPLETED` - Backup finished successfully
   - `FAILED` - Backup failed

## Manual Testing Scenarios

### Test 1: Basic Backup (No Concurrent Writes)

This tests the basic backup functionality without any concurrent modifications.

```cpp
// Create and populate database
auto db = std::make_unique<Database>("/tmp/test_db");
auto conn = std::make_unique<Connection>(db.get());
conn->query("CREATE NODE TABLE Person(name STRING, PRIMARY KEY(name))");
conn->query("CREATE (:Person {name: 'Alice'})");

// Start backup
auto storageManager = db->getStorageManager();
storageManager->startBackup("/tmp/test_backup");

// Monitor progress
while (storageManager->getBackupState() == BackupState::IN_PROGRESS) {
    double progress = storageManager->getBackupProgress();
    std::cout << "Progress: " << (progress * 100) << "%" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Check result
if (storageManager->getBackupState() == BackupState::COMPLETED) {
    std::cout << "Backup succeeded!" << std::endl;
}
```

### Test 2: Backup with Concurrent Writes

This tests the snapshot isolation and copy-on-write functionality.

```cpp
// Create database
auto db = std::make_unique<Database>("/tmp/test_db");
auto conn = std::make_unique<Connection>(db.get());

// Add initial data
conn->query("CREATE NODE TABLE Person(name STRING, PRIMARY KEY(name))");
conn->query("CREATE (:Person {name: 'Alice'})");

// Start backup
auto storageManager = db->getStorageManager();
storageManager->startBackup("/tmp/test_backup");

// Perform writes during backup
conn->query("CREATE (:Person {name: 'Bob'})");
conn->query("CREATE (:Person {name: 'Carol'})");

// Wait for backup
storageManager->waitForBackupCompletion();

// Verify: Backup should only contain Alice (snapshot isolation)
// Current DB should contain Alice, Bob, and Carol
```

### Test 3: Restore from Backup

```cpp
// Restore from backup
BackupManager::restoreFromBackup("/tmp/test_backup", "/tmp/restored_db");

// Open restored database
auto restoredDb = std::make_unique<Database>("/tmp/restored_db");
auto conn = std::make_unique<Connection>(restoredDb.get());

// Verify data
auto result = conn->query("MATCH (p:Person) RETURN p.name");
// Should only see data from backup snapshot
```

### Test 4: Multiple Backups

Test creating multiple backups sequentially:

```bash
# Create database
./build/release/tools/shell/ryu /tmp/test_db <<EOF
CREATE NODE TABLE Person(name STRING, PRIMARY KEY(name));
CREATE (:Person {name: 'Alice'});
EOF

# Backup 1
# [Trigger backup via API]

# Add more data
./build/release/tools/shell/ryu /tmp/test_db <<EOF
CREATE (:Person {name: 'Bob'});
EOF

# Backup 2
# [Trigger backup via API]

# Verify both backups contain correct data
```

### Test 5: Large Database Backup

Test backup performance with a larger dataset:

```cpp
// Create large dataset
auto db = std::make_unique<Database>("/tmp/large_db");
auto conn = std::make_unique<Connection>(db.get());
conn->query("CREATE NODE TABLE Person(id INT64, name STRING, PRIMARY KEY(id))");

// Insert many records
for (int i = 0; i < 100000; i++) {
    conn->query("CREATE (:Person {id: " + std::to_string(i) +
                ", name: 'Person" + std::to_string(i) + "'})");
}

// Time the backup
auto start = std::chrono::steady_clock::now();
auto storageManager = db->getStorageManager();
storageManager->startBackup("/tmp/large_backup");
storageManager->waitForBackupCompletion();
auto end = std::chrono::steady_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
std::cout << "Backup took " << duration.count() << " seconds" << std::endl;
```

## Expected Behavior

1. **Snapshot Isolation**: Backup should capture database state at the moment `startBackup()` is called
2. **Non-blocking**: Backup runs in background thread; database remains operational
3. **Copy-on-Write**: Pages modified during backup are preserved via shadow file
4. **Progress Tracking**: `getBackupProgress()` returns values from 0.0 to 1.0
5. **Atomicity**: Backup either completes fully or fails (no partial backups)

## Verifying Backup Success

After backup completes, verify:

1. **Backup Directory Exists**
   ```bash
   ls -la /tmp/test_backup
   ```

2. **Backup Metadata**
   - Check for metadata file with backup info
   - Verify snapshot timestamp

3. **Data Integrity**
   - Restore backup to new location
   - Query restored database
   - Compare with expected snapshot data

## Common Issues

### Issue 1: Build errors

If you get compilation errors, ensure:
- CMakeLists.txt includes backup source files
- All backup headers are in the include path

### Issue 2: "No such file or directory" during backup

Ensure:
- Backup directory path is valid
- Parent directory exists
- Write permissions are granted

### Issue 3: Backup hangs

Check:
- Background thread is running
- No deadlocks in buffer manager
- Progress updates are being called

## Test Checklist

- [ ] Basic backup completes successfully
- [ ] Backup progress updates correctly (0% → 100%)
- [ ] Backup state transitions: IDLE → IN_PROGRESS → COMPLETED
- [ ] Concurrent writes during backup don't fail
- [ ] Restored database contains snapshot data (not post-backup writes)
- [ ] Multiple sequential backups work
- [ ] Backup can be cancelled mid-operation
- [ ] Failed backup sets state to FAILED with error message
- [ ] Backup files have correct size and checksums

## Next Steps

To fully integrate backup testing:

1. **Add CMake target** for manual_backup_test
2. **Expose backup API** via C API or shell commands
3. **Add automated tests** in test/storage/backup_test.cpp
4. **Implement restore** verification logic
5. **Add backup** to CI/CD pipeline

## Files

- Manual C++ test: [examples/manual_backup_test.cpp](examples/manual_backup_test.cpp)
- Shell script test: [test_backup.sh](test_backup.sh)
- BackupManager header: [src/include/storage/backup/backup_manager.h](src/include/storage/backup/backup_manager.h)
- BackupManager impl: [src/storage/backup/backup_manager.cpp](src/storage/backup/backup_manager.cpp)
