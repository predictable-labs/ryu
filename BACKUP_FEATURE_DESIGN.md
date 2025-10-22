# Ryu Backup Feature: Zero-Downtime Design

## Executive Summary

This document outlines a comprehensive design for adding **zero-downtime backup** functionality to Ryu Graph Database. The backup feature will enable users to create consistent, point-in-time backups while the database continues to serve read and write queries without interruption.

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Design Goals](#design-goals)
3. [Architecture Overview](#architecture-overview)
4. [Implementation Strategy](#implementation-strategy)
5. [Code Changes Required](#code-changes-required)
6. [API Design](#api-design)
7. [Testing Strategy](#testing-strategy)
8. [Performance Considerations](#performance-considerations)
9. [Distributed Systems Considerations](#distributed-systems-considerations)
10. [Future Enhancements](#future-enhancements)

---

## Current State Analysis

### Existing Mechanisms in Ryu

#### 1. Checkpoint System

**Location**: `src/storage/checkpointer.cpp`, `src/storage/storage_manager.cpp`

Ryu already has a **checkpoint mechanism** that:
- Flushes in-memory changes to disk
- Uses a **ShadowFile** for copy-on-write during checkpoint
- Serializes catalog and metadata
- Ensures transactional consistency

**Key Components**:
```cpp
class Checkpointer {
    void writeCheckpoint();           // Main checkpoint entry point
    bool checkpointStorage();         // Checkpoint all tables
    void logCheckpointAndApplyShadowPages();  // Apply shadow pages
};
```

**Checkpoint Process**:
1. Create shadow pages for modified pages
2. Write new data to shadow pages
3. Update database header atomically
4. Apply shadow pages to original file
5. Clear WAL

#### 2. Shadow File Mechanism

**Location**: `src/include/storage/shadow_file.h`

The **ShadowFile** already implements copy-on-write:
```cpp
class ShadowFile {
    // Create shadow copy of page before modification
    common::page_idx_t getOrCreateShadowPage(
        common::file_idx_t originalFile,
        common::page_idx_t originalPage
    );

    // Apply shadow pages to original file
    void applyShadowPages(main::ClientContext& context);
};
```

**How it works**:
- Before modifying a page, create a shadow copy
- Write to shadow page, not original
- Original pages remain readable during checkpoint
- Atomically apply shadow pages after checkpoint

#### 3. WAL (Write-Ahead Logging)

**Location**: `src/include/storage/wal/wal.h`

- All transactions write to WAL before commit
- WAL ensures durability
- WAL is replayed on recovery

#### 4. Export/Import (CSV-based)

**Location**: `src/include/parser/port_db.h`

Ryu has `EXPORT DATABASE` and `IMPORT DATABASE` commands:
- Export to CSV files (data only, not binary)
- Import from CSV files
- **Limitation**: Not suitable for large databases (slow, lossy)

### Database File Structure

Based on analysis, Ryu stores data in:

```
<database_path>/
├── <database_name>           # Main data file (binary)
├── <database_name>.wal       # Write-Ahead Log
├── <database_name>.shadow    # Shadow file (during checkpoint)
└── <database_name>.lock      # Lock file (prevents concurrent access)
```

**File Roles**:
- **Main data file**: Contains all table data, indexes, catalog
- **WAL file**: Transaction log for durability
- **Shadow file**: Temporary file for copy-on-write during checkpoint
- **Lock file**: Prevents multiple processes from opening the same DB

---

## Design Goals

### Primary Goals

1. **Zero Downtime**: Backup must not block read or write queries
2. **Consistency**: Backup must be transactionally consistent (point-in-time snapshot)
3. **Durability**: Backup files must be complete and restorable
4. **Performance**: Minimal impact on query performance during backup
5. **Simplicity**: Easy-to-use API for users

### Non-Goals (for v1)

- Incremental backups (future enhancement)
- Cross-platform backup portability (future)
- Cloud storage integration (future)
- Encrypted backups (future)

---

## Architecture Overview

### High-Level Strategy

We'll leverage Ryu's **existing shadow file mechanism** to implement zero-downtime backups using a **copy-on-write snapshot** approach.

```
┌─────────────────────────────────────────────────────────────────┐
│                      BACKUP PROCESS                              │
└─────────────────────────────────────────────────────────────────┘

Step 1: Initiate Backup
┌──────────────────────────────────────────────────────┐
│ User calls: BACKUP DATABASE TO '/path/to/backup'    │
└───────────────────────────┬──────────────────────────┘
                            │
                            ▼
Step 2: Create Snapshot Marker
┌──────────────────────────────────────────────────────┐
│ • Record current transaction ID (snapshotTS)         │
│ • Mark start of backup in metadata                   │
│ • Create backup directory                            │
└───────────────────────────┬──────────────────────────┘
                            │
                            ▼
Step 3: Fork Background Thread
┌──────────────────────────────────────────────────────┐
│ Main Thread              │  Backup Thread            │
│ ────────────────────────────────────────────────     │
│ Continue serving         │  Copy database files      │
│ queries normally         │  • Use snapshot isolation │
│                          │  • Copy-on-write for      │
│                          │    modified pages         │
└───────────────────────────┬──────────────────────────┘
                            │
                            ▼
Step 4: Copy Database State
┌──────────────────────────────────────────────────────┐
│ Backup Thread:                                        │
│ 1. Copy main data file (with snapshot isolation)     │
│ 2. Copy WAL up to snapshotTS                         │
│ 3. Copy catalog/metadata                             │
└───────────────────────────┬──────────────────────────┘
                            │
                            ▼
Step 5: Finalize Backup
┌──────────────────────────────────────────────────────┐
│ • Verify backup integrity (checksums)                │
│ • Write backup metadata file                         │
│ • Mark backup as complete                            │
└──────────────────────────────────────────────────────┘

                    ╔═══════════════════════╗
                    ║  Backup Complete!     ║
                    ║  No downtime!         ║
                    ╚═══════════════════════╝
```

### Key Mechanisms

#### 1. Snapshot Isolation

**Concept**: Backup sees a consistent view of the database at a specific point in time (snapshot timestamp).

**Implementation**:
- Record the current max committed transaction ID as `snapshotTS`
- Backup thread reads data as of `snapshotTS`
- Pages modified after `snapshotTS` use copy-on-write

#### 2. Copy-on-Write (COW) for Active Pages

**Problem**: During backup, if a page is modified by a write transaction, we need to preserve the old version for backup.

**Solution**:
```cpp
class BackupManager {
    // Before modifying a page during backup
    void onPageModify(common::page_idx_t pageIdx) {
        if (backupInProgress && !backupCopiedPages.contains(pageIdx)) {
            // Copy original page to backup shadow file
            copyPageToBackupShadow(pageIdx);
            backupCopiedPages.insert(pageIdx);
        }
    }
};
```

**Workflow**:
1. Backup thread starts copying pages from main data file
2. If write transaction modifies a page:
   - Check if backup has already copied it
   - If not, copy original page to backup shadow file first
   - Then allow modification
3. Backup thread reads from backup shadow file if page was modified

#### 3. Incremental Page Copying

**Strategy**: Copy pages in chunks to avoid long-held locks.

```cpp
void BackupManager::copyDatabaseFile() {
    const size_t CHUNK_SIZE = 1024 * 1024;  // 1MB chunks
    auto totalPages = dataFH->getNumPages();

    for (page_idx_t pageIdx = 0; pageIdx < totalPages; pageIdx++) {
        // Copy page with snapshot isolation
        copyPageWithSnapshot(pageIdx, snapshotTS);

        // Yield periodically to avoid blocking
        if (pageIdx % 100 == 0) {
            std::this_thread::yield();
        }
    }
}
```

---

## Implementation Strategy

### Phase 1: Core Backup Infrastructure (Week 1-2)

#### 1.1 Add BackupManager Class

**Location**: `src/include/storage/backup/backup_manager.h`

```cpp
namespace ryu::storage {

enum class BackupState {
    IDLE,
    IN_PROGRESS,
    FINALIZING,
    COMPLETED,
    FAILED
};

struct BackupMetadata {
    common::transaction_t snapshotTS;     // Snapshot timestamp
    common::ku_uuid_t databaseID;         // Database ID
    std::string databasePath;             // Original DB path
    uint64_t backupTimestamp;             // Unix timestamp
    uint64_t numPages;                    // Total pages backed up
    uint64_t backupSizeBytes;             // Total size
    std::string ryuVersion;               // Ryu version

    void serialize(common::Serializer& ser) const;
    static BackupMetadata deserialize(common::Deserializer& deser);
};

class BackupManager {
public:
    BackupManager(main::ClientContext& context);
    ~BackupManager();

    // Main backup API
    void startBackup(const std::string& backupPath);
    void waitForCompletion();
    BackupState getBackupState() const;
    double getBackupProgress() const;  // 0.0 to 1.0

    // Called by write transactions
    void notifyPageModification(common::page_idx_t pageIdx);

private:
    // Background backup thread
    void backupThreadFunc();

    // Copy database files
    void copyMainDataFile();
    void copyWALFile();
    void copyMetadata();

    // Write backup metadata
    void writeBackupMetadata();

    // Verify backup integrity
    bool verifyBackupIntegrity();

private:
    main::ClientContext& clientContext;
    std::string backupPath;
    common::transaction_t snapshotTS;
    std::atomic<BackupState> state;
    std::atomic<double> progress;

    std::unique_ptr<std::thread> backupThread;
    std::mutex mtx;

    // Track which pages have been copied
    std::unordered_set<common::page_idx_t> copiedPages;

    // Shadow file for modified pages during backup
    std::unique_ptr<FileHandle> backupShadowFH;
};

} // namespace ryu::storage
```

#### 1.2 Integrate with StorageManager

**Location**: `src/include/storage/storage_manager.h`

```cpp
class StorageManager {
public:
    // ... existing methods ...

    // New backup methods
    void startBackup(const std::string& backupPath);
    BackupState getBackupState() const;
    void waitForBackupCompletion();

private:
    std::unique_ptr<BackupManager> backupManager;
};
```

#### 1.3 Hook into Page Modification

**Location**: `src/storage/buffer_manager/buffer_manager.cpp`

```cpp
uint8_t* BufferManager::pin(FileHandle& fh, common::page_idx_t pageIdx,
                              PageReadPolicy policy) {
    // ... existing pin logic ...

    // Notify backup manager if page is being modified
    auto storageManager = StorageManager::Get(context);
    if (storageManager && policy == PageReadPolicy::WRITE_PAGE) {
        storageManager->notifyBackupPageModification(pageIdx);
    }

    return frame;
}
```

### Phase 2: Snapshot Isolation for Backup (Week 2-3)

#### 2.1 Snapshot Transaction

**Concept**: Create a special read-only transaction that sees data as of `snapshotTS`.

**Location**: `src/include/transaction/transaction.h`

```cpp
class Transaction {
public:
    // ... existing methods ...

    // Mark transaction as snapshot reader (for backup)
    void setSnapshotTS(common::transaction_t snapshotTS);
    bool isSnapshotTransaction() const;

private:
    bool isSnapshot;
    common::transaction_t overrideSnapshotTS;  // For backup snapshots
};
```

#### 2.2 Snapshot-Aware Page Reads

```cpp
void BackupManager::copyPageWithSnapshot(common::page_idx_t pageIdx) {
    // Check if page was modified after snapshot
    if (isPageModifiedAfterSnapshot(pageIdx)) {
        // Read from backup shadow file (original version)
        copyFromBackupShadow(pageIdx);
    } else {
        // Read directly from main file
        copyFromMainFile(pageIdx);
    }
}
```

### Phase 3: Copy-on-Write for Modified Pages (Week 3-4)

#### 3.1 Backup Shadow File

Similar to checkpoint shadow file, but for backup:

```cpp
class BackupShadowFile {
public:
    BackupShadowFile(const std::string& backupPath,
                     BufferManager& bm);

    // Copy original page before modification
    void preserveOriginalPage(common::page_idx_t pageIdx,
                               const uint8_t* pageData);

    // Read preserved page
    void readPreservedPage(common::page_idx_t pageIdx,
                           uint8_t* buffer);

    bool hasPreservedPage(common::page_idx_t pageIdx) const;
};
```

#### 3.2 Page Modification Interception

```cpp
void BackupManager::notifyPageModification(common::page_idx_t pageIdx) {
    std::lock_guard lock{mtx};

    if (state != BackupState::IN_PROGRESS) {
        return;  // No backup in progress
    }

    // If we haven't copied this page yet, preserve original
    if (!copiedPages.contains(pageIdx)) {
        auto* page = getOriginalPageData(pageIdx);
        backupShadowFile->preserveOriginalPage(pageIdx, page);
    }
}
```

### Phase 4: WAL Handling (Week 4)

#### 4.1 WAL Snapshot

Copy WAL entries up to snapshot timestamp:

```cpp
void BackupManager::copyWALFile() {
    auto& wal = storageManager->getWAL();

    // Read WAL file
    auto walReader = WAL::openForReplay(databasePath, true);
    auto backupWalPath = backupPath + ".wal";
    auto backupWalWriter = createBackupWALWriter(backupWalPath);

    // Copy WAL records up to snapshotTS
    while (walReader->hasMoreRecords()) {
        auto record = walReader->readNextRecord();

        if (record->getTransactionID() <= snapshotTS) {
            backupWalWriter->writeRecord(record);
        } else {
            break;  // Beyond snapshot
        }
    }

    backupWalWriter->flush();
}
```

### Phase 5: Backup Verification (Week 4-5)

#### 5.1 Integrity Checks

```cpp
bool BackupManager::verifyBackupIntegrity() {
    // 1. Verify all pages were copied
    auto expectedPages = dataFH->getNumPages();
    auto actualPages = countBackupPages();
    if (expectedPages != actualPages) {
        return false;
    }

    // 2. Verify checksums
    for (page_idx_t i = 0; i < expectedPages; i++) {
        if (!verifyPageChecksum(i)) {
            return false;
        }
    }

    // 3. Verify metadata
    if (!verifyBackupMetadata()) {
        return false;
    }

    return true;
}
```

### Phase 6: Restore Functionality (Week 5-6)

#### 6.1 Restore API

```cpp
class BackupManager {
public:
    // Restore from backup
    static void restoreFromBackup(const std::string& backupPath,
                                   const std::string& targetDbPath);

private:
    static void copyBackupToTarget(const std::string& backupPath,
                                    const std::string& targetDbPath);
    static void verifyRestoreIntegrity(const std::string& targetDbPath);
};
```

---

## Code Changes Required

### New Files to Create

```
src/include/storage/backup/
├── backup_manager.h          # Main backup manager
├── backup_metadata.h         # Backup metadata structures
├── backup_shadow_file.h      # Shadow file for backup
└── backup_state.h            # Backup state enums

src/storage/backup/
├── backup_manager.cpp        # Implementation
├── backup_metadata.cpp
├── backup_shadow_file.cpp
└── restore.cpp              # Restore functionality

src/include/parser/
└── backup_statement.h        # Parser for BACKUP/RESTORE commands

src/processor/operator/
├── physical_backup.h         # Physical operator for backup
└── physical_backup.cpp
```

### Files to Modify

#### 1. Storage Manager

**File**: `src/include/storage/storage_manager.h`

```cpp
class StorageManager {
public:
    // ... existing ...

    // Add backup methods
    void startBackup(const std::string& backupPath);
    BackupState getBackupState() const;
    double getBackupProgress() const;
    void waitForBackupCompletion();
    void cancelBackup();

    // Called by buffer manager on page modifications
    void notifyBackupPageModification(common::page_idx_t pageIdx);

private:
    std::unique_ptr<BackupManager> backupManager;
};
```

**Implementation**: `src/storage/storage_manager.cpp`

```cpp
void StorageManager::startBackup(const std::string& backupPath) {
    if (!backupManager) {
        backupManager = std::make_unique<BackupManager>(clientContext);
    }
    backupManager->startBackup(backupPath);
}

BackupState StorageManager::getBackupState() const {
    return backupManager ? backupManager->getBackupState() : BackupState::IDLE;
}

void StorageManager::notifyBackupPageModification(common::page_idx_t pageIdx) {
    if (backupManager && backupManager->getBackupState() == BackupState::IN_PROGRESS) {
        backupManager->notifyPageModification(pageIdx);
    }
}
```

#### 2. Buffer Manager

**File**: `src/storage/buffer_manager/buffer_manager.cpp`

```cpp
uint8_t* BufferManager::pin(FileHandle& fh, common::page_idx_t pageIdx,
                              PageReadPolicy policy) {
    // ... existing pin logic ...

    // Notify backup of page modification
    if (policy == PageReadPolicy::WRITE_PAGE) {
        auto* storageManager = StorageManager::Get(currentContext);
        if (storageManager) {
            storageManager->notifyBackupPageModification(pageIdx);
        }
    }

    return frame;
}
```

#### 3. Transaction Manager

**File**: `src/include/transaction/transaction.h`

```cpp
class Transaction {
public:
    // ... existing ...

    void setSnapshotTS(common::transaction_t ts) {
        isSnapshot = true;
        overrideSnapshotTS = ts;
    }

    common::transaction_t getEffectiveStartTS() const {
        return isSnapshot ? overrideSnapshotTS : startTS;
    }

private:
    bool isSnapshot = false;
    common::transaction_t overrideSnapshotTS = 0;
};
```

#### 4. Parser (Cypher Grammar)

**File**: `src/antlr4/Cypher.g4`

Add backup/restore syntax:

```antlr
backupStatement
    : BACKUP DATABASE TO stringLiteral
    ;

restoreStatement
    : RESTORE DATABASE FROM stringLiteral (TO stringLiteral)?
    ;
```

#### 5. Connection API

**File**: `src/include/main/connection.h`

```cpp
class Connection {
public:
    // ... existing ...

    // Backup API
    void startBackup(const std::string& backupPath);
    BackupState getBackupState() const;
    double getBackupProgress() const;
    void waitForBackupCompletion();
};
```

---

## API Design

### Cypher Syntax

#### Create Backup

```cypher
BACKUP DATABASE TO '/path/to/backup';
```

**Options** (future):
```cypher
BACKUP DATABASE TO '/path/to/backup'
  OPTIONS (
    compression = true,
    verify = true
  );
```

#### Restore Backup

```cypher
RESTORE DATABASE FROM '/path/to/backup';
```

**Options**:
```cypher
RESTORE DATABASE FROM '/path/to/backup' TO '/new/database/path';
```

#### Check Backup Status

```cypher
SHOW BACKUP STATUS;
```

Returns:
```
┌─────────────┬──────────┬────────────────────────┐
│ state       │ progress │ elapsed_time           │
├─────────────┼──────────┼────────────────────────┤
│ IN_PROGRESS │ 45.2%    │ 00:02:15               │
└─────────────┴──────────┴────────────────────────┘
```

### Programmatic API

#### C++ API

```cpp
// Create backup
auto conn = std::make_unique<Connection>(database);
conn->startBackup("/path/to/backup");

// Wait for completion
conn->waitForBackupCompletion();

// Check progress
auto progress = conn->getBackupProgress();
std::cout << "Backup progress: " << (progress * 100) << "%" << std::endl;
```

#### Python API

```python
import ryudb

db = ryudb.Database("mydb.db")
conn = ryudb.Connection(db)

# Start backup (non-blocking)
conn.start_backup("/path/to/backup")

# Monitor progress
while conn.get_backup_state() == ryudb.BackupState.IN_PROGRESS:
    progress = conn.get_backup_progress()
    print(f"Backup progress: {progress * 100:.1f}%")
    time.sleep(1)

# Or wait for completion (blocking)
conn.wait_for_backup_completion()
```

#### Rust API

```rust
use ryudb::{Database, Connection};

let db = Database::new("mydb.db")?;
let conn = db.connect()?;

// Start backup
conn.start_backup("/path/to/backup")?;

// Wait for completion
conn.wait_for_backup_completion()?;
```

---

## Testing Strategy

### Unit Tests

#### 1. Backup Manager Tests

**File**: `test/storage/backup_manager_test.cpp`

```cpp
TEST_F(BackupManagerTest, BasicBackup) {
    // Setup: Create database with data
    createTestDatabase();

    // Execute: Start backup
    storageManager->startBackup("/tmp/test_backup");
    storageManager->waitForBackupCompletion();

    // Verify: Backup completed successfully
    EXPECT_EQ(storageManager->getBackupState(), BackupState::COMPLETED);
    EXPECT_TRUE(backupExists("/tmp/test_backup"));
}

TEST_F(BackupManagerTest, BackupDuringWrites) {
    // Setup: Create database
    createTestDatabase();

    // Execute: Start backup
    storageManager->startBackup("/tmp/test_backup");

    // Concurrently: Insert data while backup is running
    std::thread writerThread([&]() {
        for (int i = 0; i < 1000; i++) {
            conn->query("CREATE (p:Person {id: " + std::to_string(i) + "})");
        }
    });

    // Wait for backup
    storageManager->waitForBackupCompletion();
    writerThread.join();

    // Verify: Backup is consistent (no partial writes)
    verifyBackupConsistency("/tmp/test_backup");
}

TEST_F(BackupManagerTest, RestoreFromBackup) {
    // Create and backup database
    createTestDatabase();
    storageManager->startBackup("/tmp/test_backup");
    storageManager->waitForBackupCompletion();

    // Restore to new location
    BackupManager::restoreFromBackup("/tmp/test_backup", "/tmp/restored_db");

    // Verify restored database matches original
    verifyDatabasesMatch("test.db", "/tmp/restored_db");
}
```

#### 2. Concurrent Access Tests

```cpp
TEST_F(BackupManagerTest, ReadsDuringBackup) {
    // Verify reads are not blocked during backup
    createTestDatabase();
    storageManager->startBackup("/tmp/test_backup");

    // Execute queries while backup is running
    auto result = conn->query("MATCH (p:Person) RETURN count(*)");
    EXPECT_TRUE(result->hasNext());

    storageManager->waitForBackupCompletion();
}

TEST_F(BackupManagerTest, WritesDuringBackup) {
    // Verify writes are not blocked during backup
    createTestDatabase();
    storageManager->startBackup("/tmp/test_backup");

    // Execute writes while backup is running
    conn->query("CREATE (p:Person {name: 'Alice'})");

    storageManager->waitForBackupCompletion();

    // Verify write was successful
    auto result = conn->query("MATCH (p:Person {name: 'Alice'}) RETURN p");
    EXPECT_TRUE(result->hasNext());
}
```

### Integration Tests

#### 1. End-to-End Backup/Restore

**File**: `test/integration/backup_integration_test.cpp`

```cpp
TEST_F(BackupIntegrationTest, FullBackupRestoreCycle) {
    // 1. Create database with complex schema
    createComplexDatabase();  // Nodes, relationships, indexes

    // 2. Backup
    conn->query("BACKUP DATABASE TO '/tmp/integration_backup'");

    // 3. Modify original database
    conn->query("CREATE (p:Person {name: 'Bob'})");

    // 4. Restore to new database
    auto restoredDb = Database("/tmp/restored_integration_db");
    BackupManager::restoreFromBackup("/tmp/integration_backup",
                                      "/tmp/restored_integration_db");
    auto restoredConn = Connection(&restoredDb);

    // 5. Verify restored database doesn't have 'Bob' (from step 3)
    auto result = restoredConn.query("MATCH (p:Person {name: 'Bob'}) RETURN p");
    EXPECT_FALSE(result->hasNext());

    // 6. Verify restored database has original data
    verifyOriginalData(restoredConn);
}
```

### Performance Tests

#### 1. Backup Performance

```cpp
TEST_F(BackupPerformanceTest, LargeDatabaseBackup) {
    // Create 1M nodes, 5M relationships
    createLargeDatabase(1000000, 5000000);

    auto start = std::chrono::steady_clock::now();
    storageManager->startBackup("/tmp/perf_backup");
    storageManager->waitForBackupCompletion();
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Backup should complete within reasonable time
    EXPECT_LT(duration.count(), 300);  // < 5 minutes for 1M nodes

    std::cout << "Backup time: " << duration.count() << " seconds" << std::endl;
}
```

#### 2. Query Performance During Backup

```cpp
TEST_F(BackupPerformanceTest, QueryLatencyDuringBackup) {
    createTestDatabase();

    // Measure query latency without backup
    auto baselineLatency = measureQueryLatency();

    // Start backup
    storageManager->startBackup("/tmp/perf_backup");

    // Measure query latency during backup
    auto backupLatency = measureQueryLatency();

    // Latency increase should be < 20%
    auto increase = (backupLatency - baselineLatency) / baselineLatency;
    EXPECT_LT(increase, 0.20);

    storageManager->waitForBackupCompletion();
}
```

---

## Performance Considerations

### Expected Impact

#### During Backup

1. **CPU Usage**: +10-20%
   - Background thread copying pages
   - Minimal impact on query threads

2. **Memory Usage**: +5-10%
   - Backup shadow file pages
   - Tracking copied pages

3. **Disk I/O**: +50-100%
   - Reading all pages for backup
   - Writing to backup file
   - But uses separate I/O thread, minimal query impact

4. **Query Latency**: +5-10%
   - Copy-on-write overhead for modified pages
   - Minimal for most workloads

### Optimizations

#### 1. Adaptive Throttling

```cpp
class BackupManager {
    void adaptiveThrottle() {
        // Monitor system load
        auto cpuUsage = getCPUUsage();
        auto diskIOUsage = getDiskIOUsage();

        // Slow down backup if system is under load
        if (cpuUsage > 0.8 || diskIOUsage > 0.8) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};
```

#### 2. Page Copy Batching

```cpp
void BackupManager::copyPages() {
    const size_t BATCH_SIZE = 100;
    std::vector<common::page_idx_t> batch;

    for (auto pageIdx : pagesToCopy) {
        batch.push_back(pageIdx);

        if (batch.size() >= BATCH_SIZE) {
            copyPageBatch(batch);
            batch.clear();
            adaptiveThrottle();
        }
    }
}
```

#### 3. Compression (Future)

```cpp
// Option to compress backup files
BACKUP DATABASE TO '/path/to/backup'
  OPTIONS (compression = true);
```

---

## Distributed Systems Considerations

### Current Scope: Single-Node Deployment

This backup design is **optimized for single-node embedded Ryu deployments**. Ryu currently does not have distributed replication or Raft consensus support.

**Key Characteristics**:
- ✅ **Embedded**: Ryu runs in the same process as the application
- ✅ **Single-node**: No distributed state or replication
- ✅ **ACID**: Via MVCC + WAL (single node)
- ❌ **High availability**: Not currently supported (no replicas)

### Future: Raft Consensus Integration

For distributed Ryu deployments with Raft consensus, see **[RAFT_CONSENSUS_INTEGRATION.md](RAFT_CONSENSUS_INTEGRATION.md)** for detailed analysis.

#### Raft Backup Strategies

When Raft is added to Ryu, the backup feature can be extended to support:

**1. Leader-Only Backup** (Simple, recommended for v2)
```
Raft Cluster → Only leader performs backup
- Simple integration
- Backup consistent with committed state
- Minimal overhead
```

**2. Follower Backup** (Offload leader)
```
Raft Cluster → Designated follower performs backup
- Offloads work from leader
- Leader performance unaffected
- Good for high query loads
```

**3. Coordinated Cluster-Wide Backup** (Advanced, v3)
```
Raft Cluster → All nodes participate in backup
- True cluster-wide consistency
- Survives any single node failure
- Requires backup coordination protocol
```

#### Design Forward-Compatibility

The current single-node backup design **is forward-compatible** with Raft:

**Reusable Components**:
- ✅ Snapshot isolation mechanism
- ✅ Copy-on-write for modified pages
- ✅ WAL snapshot copying
- ✅ Backup verification
- ✅ Restore from backup

**Required Extensions for Raft**:
- 🔄 Role awareness (leader/follower check)
- 🔄 Raft log index in backup metadata
- 🔄 Backup coordination protocol (for cluster-wide backup)
- 🔄 Integration with Raft snapshot mechanism

**Metadata Extension for Raft**:
```cpp
struct BackupMetadata {
    // Existing fields (single-node)
    common::transaction_t snapshotTS;
    common::ku_uuid_t databaseID;
    std::string databasePath;
    uint64_t backupTimestamp;

    // Future: Raft-specific fields
    std::optional<uint64_t> raftLogIndex;      // Raft log position
    std::optional<std::string> nodeRole;        // "leader" or "follower"
    std::optional<std::string> clusterId;       // Cluster identifier
};
```

#### Roadmap: Single-Node → Distributed

```
Phase 1 (This Design): Single-Node Backup
├─ Zero-downtime backup for embedded Ryu
├─ Snapshot isolation + COW
└─ 6-week implementation
        │
        ▼
Phase 2 (Future): Raft Integration
├─ Add Raft consensus to Ryu
├─ Leader-only backup (simple)
└─ 8-10 week implementation
        │
        ▼
Phase 3 (Advanced): Coordinated Cluster Backup
├─ Backup coordination protocol
├─ Cross-node consistency verification
└─ 4-6 week implementation
```

**Recommendation**:
- ✅ **Implement Phase 1 now** (single-node backup) - provides immediate value for embedded use cases
- 🔄 **Evaluate Raft** based on user requirements for distributed deployments
- 🚀 **Extend to Raft** if/when distributed Ryu is needed

For detailed Raft integration design, architecture comparisons, and implementation roadmap, see [RAFT_CONSENSUS_INTEGRATION.md](RAFT_CONSENSUS_INTEGRATION.md).

---

## Future Enhancements

### Phase 2 Features (Post-MVP)

#### 1. Incremental Backups

**Concept**: Only backup pages modified since last backup.

```cpp
struct IncrementalBackupMetadata {
    std::string baseBackupPath;          // Full backup path
    common::transaction_t baseSnapshotTS;  // Base backup snapshot
    std::vector<common::page_idx_t> modifiedPages;  // Pages changed since base
};

// Usage
BACKUP DATABASE TO '/path/to/backup'
  OPTIONS (incremental = true, base = '/path/to/full_backup');
```

#### 2. Scheduled Backups

```cpp
// Create backup schedule
CREATE BACKUP SCHEDULE daily_backup
  TO '/backups/daily'
  EVERY 24 HOURS;

// List schedules
SHOW BACKUP SCHEDULES;

// Drop schedule
DROP BACKUP SCHEDULE daily_backup;
```

#### 3. Cloud Storage Integration

```cpp
// Backup to S3
BACKUP DATABASE TO 's3://mybucket/backups/db_backup'
  OPTIONS (
    aws_access_key = '...',
    aws_secret_key = '...'
  );

// Backup to Azure Blob
BACKUP DATABASE TO 'azure://mycontainer/backups/db_backup'
  OPTIONS (
    azure_account = '...',
    azure_key = '...'
  );
```

#### 4. Encrypted Backups

```cpp
BACKUP DATABASE TO '/path/to/backup'
  OPTIONS (
    encryption = true,
    encryption_key = '...'
  );
```

#### 5. Backup Retention Policies

```cpp
CREATE BACKUP POLICY keep_30_days
  RETENTION 30 DAYS
  AUTO_DELETE true;

BACKUP DATABASE TO '/path/to/backup'
  WITH POLICY keep_30_days;
```

#### 6. Point-in-Time Recovery (PITR)

```cpp
// Restore to specific timestamp
RESTORE DATABASE FROM '/path/to/backup'
  TO TIMESTAMP '2025-10-21 14:30:00';

// Restore to specific transaction
RESTORE DATABASE FROM '/path/to/backup'
  TO TRANSACTION 12345;
```

---

## Implementation Timeline

### Phase 1: MVP (6 weeks)

**Week 1-2**: Core infrastructure
- BackupManager class
- Snapshot isolation
- Basic file copying

**Week 3-4**: Copy-on-write mechanism
- Backup shadow file
- Page modification hooks
- Concurrent backup support

**Week 5**: WAL handling & verification
- WAL snapshot copying
- Integrity checks
- Error handling

**Week 6**: Testing & polish
- Unit tests
- Integration tests
- Performance optimization

### Phase 2: Advanced Features (4-6 weeks, post-MVP)

- Incremental backups
- Compression
- Cloud storage integration
- Scheduled backups

---

## Risk Mitigation

### Potential Issues

#### 1. Backup Corruption

**Risk**: Backup file corrupted due to system crash during backup.

**Mitigation**:
- Write backup metadata last
- Atomic file operations
- Integrity checksums
- Verify backup before marking complete

#### 2. Out of Disk Space

**Risk**: Backup fails due to insufficient disk space.

**Mitigation**:
```cpp
void BackupManager::startBackup(const std::string& backupPath) {
    // Check available disk space
    auto requiredSpace = estimateBackupSize();
    auto availableSpace = getAvailableDiskSpace(backupPath);

    if (availableSpace < requiredSpace * 1.1) {  // 10% buffer
        throw BackupException("Insufficient disk space for backup");
    }

    // Proceed with backup
}
```

#### 3. Performance Degradation

**Risk**: Backup causes unacceptable query slowdown.

**Mitigation**:
- Adaptive throttling
- I/O priority tuning
- Configurable backup rate limits

#### 4. Long-Running Backups

**Risk**: Backup takes too long for very large databases.

**Mitigation**:
- Progress reporting
- Ability to cancel backup
- Resume from checkpoint (future)

---

## Summary

This design provides:

✅ **Zero downtime**: Backup runs in background, queries not blocked
✅ **Consistency**: Point-in-time snapshot with MVCC
✅ **Durability**: Verified, restorable backups
✅ **Performance**: Minimal impact (<10% latency increase)
✅ **Simplicity**: Easy-to-use API (`BACKUP DATABASE TO '...'`)
✅ **Forward-compatible**: Design extends naturally to Raft-based distributed deployments

**Key Innovation**: Leverage existing shadow file mechanism for copy-on-write during backup, ensuring zero downtime while maintaining consistency.

**Current Scope**: Single-node embedded Ryu deployments (Ryu does not currently have Raft/distributed support)

**Future Extensibility**: The design is forward-compatible with Raft consensus. See [Distributed Systems Considerations](#distributed-systems-considerations) and [RAFT_CONSENSUS_INTEGRATION.md](RAFT_CONSENSUS_INTEGRATION.md) for details on how this backup feature will integrate with future distributed Ryu deployments.

**Next Steps**: Review and approve design, then proceed with Phase 1 implementation (single-node backup).

---

## Appendix: Example Usage

### Basic Backup

```cypher
-- Connect to database
USE mydb;

-- Create backup
BACKUP DATABASE TO '/backups/mydb_2025_10_21';

-- Verify backup completed
SHOW BACKUP STATUS;
```

### Backup with Progress Monitoring

```python
import ryudb
import time

db = ryudb.Database("mydb.db")
conn = ryudb.Connection(db)

# Start backup
conn.start_backup("/backups/mydb_2025_10_21")

# Monitor progress
while conn.get_backup_state() == ryudb.BackupState.IN_PROGRESS:
    progress = conn.get_backup_progress()
    print(f"Backup progress: {progress * 100:.1f}%")
    time.sleep(2)

print("Backup completed!")
```

### Restore from Backup

```cypher
-- Restore to new database
RESTORE DATABASE FROM '/backups/mydb_2025_10_21' TO '/data/mydb_restored';
```

### Backup During Heavy Workload

```python
import ryudb
import threading

db = ryudb.Database("mydb.db")
conn = ryudb.Connection(db)

# Start backup in background
conn.start_backup("/backups/mydb_backup")

# Continue inserting data
def insert_data():
    for i in range(10000):
        conn.query(f"CREATE (p:Person {{id: {i}}})")

writer_thread = threading.Thread(target=insert_data)
writer_thread.start()

# Wait for backup
conn.wait_for_backup_completion()
writer_thread.join()

print("Backup completed while inserting 10,000 nodes!")
```

---

*End of Backup Feature Design Document*
