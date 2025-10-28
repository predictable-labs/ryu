# Ryu Graph Database: In-Depth Architecture Analysis

## Table of Contents

1. [Overview](#overview)
2. [Architecture Fundamentals](#architecture-fundamentals)
3. [Storage Layer](#storage-layer)
4. [Query Processing Pipeline](#query-processing-pipeline)
5. [Transaction Management](#transaction-management)
6. [Indexing System](#indexing-system)
7. [Buffer Management](#buffer-management)
8. [Write-Ahead Logging](#write-ahead-logging)
9. [Memory Management](#memory-management)
10. [Extension System](#extension-system)
11. [API & Language Bindings](#api--language-bindings)
12. [Performance Optimizations](#performance-optimizations)
13. [Key Innovations](#key-innovations)

---

## Overview

**Ryu** (formerly Kuzu) is an embedded graph database built in C++20, designed for high-performance analytical graph workloads. The codebase consists of approximately **102,000 lines of C++ code** organized into a modular, layered architecture.

### Core Design Principles

- **Embeddable**: Runs in-process without requiring a separate server
- **ACID Compliant**: Full transactional support with serializable isolation
- **Columnar Storage**: Optimized for analytical workloads
- **Vectorized Execution**: SIMD-friendly batch processing
- **Multi-threaded**: Parallel query execution
- **Extensible**: Plugin architecture for custom functionality

### Technology Stack

- **Language**: C++20
- **Parser**: ANTLR4 for Cypher query parsing
- **Build System**: CMake
- **Platforms**: Linux, macOS, Windows (x86_64, ARM64)
- **License**: MIT (formerly from Kuzu Inc., now maintained by Predictable Labs)

---

## Architecture Fundamentals

### Component Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      Client Application                      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Connection (Thread-Safe)                   │
│  • Query Execution  • Prepared Statements  • UDF Support    │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                         Database                             │
│ ┌──────────┬──────────┬──────────┬──────────┬─────────────┐ │
│ │ Catalog  │ Storage  │  Query   │Transaction│  Extension  │ │
│ │ Manager  │ Manager  │Processor │  Manager  │   Manager   │ │
│ └──────────┴──────────┴──────────┴──────────┴─────────────┘ │
└────────────────────────┬────────────────────────────────────┘
                         │
         ┌───────────────┼───────────────┐
         ▼               ▼               ▼
    ┌────────┐     ┌─────────┐     ┌─────────┐
    │ Buffer │     │   WAL   │     │  VFS    │
    │Manager │     │         │     │         │
    └────────┘     └─────────┘     └─────────┘
         │
         ▼
    ┌─────────────────────────────────┐
    │     Disk Storage (Tables)        │
    └─────────────────────────────────┘
```

### Database Class

**Location**: `src/include/main/database.h:95-200`

The `Database` class is the main entry point and manages all database components:

```cpp
class Database {
public:
    Database(std::string_view databasePath, SystemConfig systemConfig);

private:
    std::unique_ptr<catalog::Catalog> catalog;
    std::unique_ptr<storage::StorageManager> storageManager;
    std::unique_ptr<processor::QueryProcessor> queryProcessor;
    std::unique_ptr<transaction::TransactionManager> transactionManager;
    std::unique_ptr<storage::BufferManager> bufferManager;
    std::unique_ptr<storage::MemoryManager> memoryManager;
    std::unique_ptr<extension::ExtensionManager> extensionManager;
    std::unique_ptr<common::VirtualFileSystem> vfs;
};
```

### SystemConfig Parameters

**Location**: `src/include/main/database.h:38-90`

```cpp
struct SystemConfig {
    uint64_t bufferPoolSize;          // Max memory (default: 80% of available RAM)
    uint64_t maxNumThreads;           // Parallelism (default: hardware threads)
    bool enableCompression;           // On-disk compression (default: true)
    bool readOnly;                    // Read-only mode (default: false)
    uint64_t maxDBSize;               // Max database size (default: 8TB)
    bool autoCheckpoint;              // Auto-checkpoint WAL (default: true)
    uint64_t checkpointThreshold;     // Checkpoint trigger (default: 16MB)
    bool forceCheckpointOnClose;      // Checkpoint on close (default: true)
    bool throwOnWalReplayFailure;     // Error on WAL replay failure (default: true)
    bool enableChecksums;             // WAL checksums (default: true)
};
```

### Connection Class

**Location**: `src/include/main/connection.h:14-158`

Provides thread-safe query execution interface:

```cpp
class Connection {
public:
    explicit Connection(Database* database);

    // Query execution
    std::unique_ptr<QueryResult> query(std::string_view query);
    std::unique_ptr<PreparedStatement> prepare(std::string_view query);
    std::unique_ptr<QueryResult> execute(PreparedStatement* stmt, ...);

    // UDF support
    template<typename TR, typename... Args>
    void createScalarFunction(std::string name, TR (*udfFunc)(Args...));

    // Control
    void interrupt();
    void setQueryTimeOut(uint64_t timeoutInMS);
    void setMaxNumThreadForExec(uint64_t numThreads);
};
```

---

## Storage Layer

The storage layer is organized hierarchically: **Tables → NodeGroups → ColumnChunks → Segments**.

### 3-Level Hierarchical Organization

```
Table (NodeTable/RelTable)
  │
  ├─ Column 1
  │   ├─ NodeGroup 0 (2048 rows)
  │   │   ├─ ColumnChunk
  │   │   │   ├─ Segment 1
  │   │   │   └─ Segment 2
  │   │   └─ UpdateInfo (MVCC)
  │   ├─ NodeGroup 1
  │   └─ NodeGroup N
  │
  ├─ Column 2
  └─ Primary Key Index
```

### Level 1: Tables

**Location**: `src/include/storage/table/node_table.h:110-228`

#### NodeTable (Vertex Storage)

```cpp
class NodeTable final : public Table {
public:
    // Scanning
    void initScanState(transaction::Transaction* txn, TableScanState& state);
    bool scanInternal(transaction::Transaction* txn, TableScanState& state);
    bool lookup(const transaction::Transaction* txn, const TableScanState& state);

    // Modification
    void insert(transaction::Transaction* txn, TableInsertState& state);
    void update(transaction::Transaction* txn, TableUpdateState& state);
    bool delete_(transaction::Transaction* txn, TableDeleteState& state);

    // Indexing
    void addIndex(std::unique_ptr<Index> index);
    PrimaryKeyIndex* getPKIndex() const;

private:
    std::vector<std::unique_ptr<Column>> columns;
    std::unique_ptr<NodeGroupCollection> nodeGroups;
    common::column_id_t pkColumnID;
    std::vector<IndexHolder> indexes;
};
```

**Key Features**:
- Each node table has a mandatory **primary key**
- Columns store property values in columnar format
- **NodeGroupCollection** manages horizontal partitioning
- Supports multiple secondary indexes

### Level 2: NodeGroups

**Location**: `src/include/storage/table/node_group.h:85-267`

NodeGroups partition table data horizontally (default: 2048 rows per group):

```cpp
class NodeGroup {
public:
    NodeGroup(MemoryManager& mm, common::node_group_idx_t nodeGroupIdx,
              bool enableCompression, std::vector<common::LogicalType> dataTypes,
              common::row_idx_t capacity = 2048);

    // Data access
    NodeGroupScanResult scan(const transaction::Transaction* txn, TableScanState& state);
    bool lookup(const transaction::Transaction* txn, const TableScanState& state);

    // Modification
    common::row_idx_t append(const transaction::Transaction* txn,
                              const std::vector<common::column_id_t>& columnIDs,
                              ChunkedNodeGroup& chunkedGroup, ...);
    void update(const transaction::Transaction* txn, common::row_idx_t rowIdx,
                common::column_id_t columnID, const common::ValueVector& vector);
    bool delete_(const transaction::Transaction* txn, common::row_idx_t rowIdx);

    // Versioning (MVCC)
    bool isVisible(const transaction::Transaction* txn, common::row_idx_t rowIdx);

    // Persistence
    void checkpoint(MemoryManager& mm, NodeGroupCheckpointState& state);

private:
    std::atomic<common::row_idx_t> numRows;
    common::row_idx_t capacity;
    std::vector<common::LogicalType> dataTypes;
    GroupCollection<ChunkedNodeGroup> chunkedGroups;  // Committed but not checkpointed
};
```

**Key Concepts**:
- **ChunkedNodeGroups**: Store committed but not yet checkpointed data
- **Atomic row count**: Thread-safe row tracking
- **Version information**: Supports MVCC visibility checks
- **Checkpoint state**: Manages incremental checkpointing

### Level 3: ColumnChunks

**Location**: `src/include/storage/table/column_chunk.h:141-334`

The actual data storage unit:

```cpp
class ColumnChunk {
public:
    // Construction
    ColumnChunk(MemoryManager& mm, common::LogicalType&& dataType,
                uint64_t capacity, bool enableCompression,
                ResidencyState residencyState);

    // Data access
    void scan(const transaction::Transaction* txn, const ChunkState& state,
              common::ValueVector& output, common::offset_t offset,
              common::length_t length);
    void lookup(const transaction::Transaction* txn, const ChunkState& state,
                common::offset_t rowInChunk, common::ValueVector& output,
                common::sel_t posInOutputVector);

    // Modification
    void update(const transaction::Transaction* txn, common::offset_t offset,
                const common::ValueVector& values);
    void append(common::ValueVector* vector, const common::SelectionView& selView);

    // Compression & Persistence
    void flush(PageAllocator& pageAllocator);
    void checkpoint(Column& column, std::vector<ChunkCheckpointState>&& states,
                    PageAllocator& pageAllocator);

    // State
    ResidencyState getResidencyState() const;  // IN_MEMORY or ON_DISK
    bool hasUpdates() const;

private:
    bool enableCompression;
    std::vector<std::unique_ptr<ColumnChunkData>> data;  // Segments
    UpdateInfo updateInfo;  // MVCC updates
};
```

**Segment-based Storage**:
- Data is split into **segments** for efficient I/O
- Segments can be compressed independently
- Supports lazy loading (on-disk segments loaded on demand)

**Residency States**:
- `IN_MEMORY`: Data resident in memory
- `ON_DISK`: Data must be loaded from disk

### CSR Format for Relationships

**Location**: `src/include/storage/table/csr_node_group.h:21-97`

For graph edges, Ryu uses **Compressed Sparse Row (CSR)** format for efficient adjacency list storage:

```cpp
struct NodeCSRIndex {
    bool isSequential;           // Optimization flag
    row_idx_vec_t rowIndices;    // Either [start, length] or explicit indices

    common::row_idx_t getNumRows() const {
        if (isSequential) {
            return rowIndices[1];  // rowIndices = [startRow, length]
        }
        return rowIndices.size();
    }

    row_idx_vec_t getRows() const {
        if (isSequential) {
            // Generate range [startRow, startRow + length)
            row_idx_vec_t result;
            for (auto i = 0u; i < rowIndices[1]; ++i) {
                result.push_back(rowIndices[0] + i);
            }
            return result;
        }
        return rowIndices;  // Already explicit
    }
};

struct CSRIndex {
    // One index per node in the node group (2048 entries)
    std::array<NodeCSRIndex, 2048> indices;
};
```

**CSR Benefits**:
- **O(1) adjacency access**: Given a node, instantly find its neighbors
- **Space-efficient**: Sequential relationships stored as (start, length)
- **Cache-friendly**: Relationships stored contiguously

**Example**:
```
Node 0: neighbors at rows [10, 11, 12, 13, 14]
  → isSequential=true, rowIndices=[10, 5]

Node 1: neighbors at rows [20, 25, 30]
  → isSequential=false, rowIndices=[20, 25, 30]
```

---

## Query Processing Pipeline

### Overview

Queries flow through 5 stages:

```
Query String
    ↓
[1. PARSING] ─────→ AST (Abstract Syntax Tree)
    ↓
[2. BINDING] ─────→ Bound Statement (typed, resolved)
    ↓
[3. PLANNING] ────→ Logical Plan
    ↓
[4. OPTIMIZATION] → Optimized Logical Plan
    ↓
[5. EXECUTION] ───→ Physical Plan → Results
```

### Stage 1: Parsing

**Location**: `src/include/parser/parser.h:16-24`

Uses **ANTLR4** to parse Cypher queries:

```cpp
class Parser {
public:
    static std::vector<std::shared_ptr<Statement>> parseQuery(
        std::string_view query,
        std::vector<extension::TransformerExtension*> transformerExtensions = {}
    );
};
```

**Output**: Abstract Syntax Tree (AST) representing the query structure.

### Stage 2: Binding

**Location**: `src/include/binder/binder.h:67-80`

Resolves symbols and validates semantics:

```cpp
class Binder {
public:
    std::unique_ptr<BoundStatement> bind(const parser::Statement& statement);

    std::shared_ptr<Expression> createVariable(const std::string& name,
                                                const common::LogicalType& dataType);
private:
    BinderScope scope;                    // Variable scoping
    ExpressionBinder expressionBinder;    // Expression resolution
    main::ClientContext* clientContext;   // Access to catalog
};
```

**Responsibilities**:
- Resolve table/column names to catalog entries
- Type checking and inference
- Variable scoping (WITH clauses, subqueries)
- Convert parsed expressions to typed expressions

**Example**:
```cypher
MATCH (p:Person)-[:KNOWS]->(f:Person)
WHERE p.age > 25
RETURN p.name, f.name
```

Binding:
- `Person` → resolved to NodeTable in catalog
- `KNOWS` → resolved to RelTable in catalog
- `p.age` → resolved to property column, type INT64
- `p.name`, `f.name` → resolved to property columns, type STRING

### Stage 3: Planning

**Location**: `src/include/planner/planner.h`

Creates a **logical query plan**:

```cpp
class Planner {
public:
    std::unique_ptr<LogicalPlan> getBestPlan(BoundStatement& statement);
};
```

**Logical Operators** (examples):
- `LogicalScanNodeTable`: Scan node table
- `LogicalExtend`: Traverse relationships
- `LogicalFilter`: Apply predicates
- `LogicalHashJoin`: Join two streams
- `LogicalAggregate`: Group and aggregate
- `LogicalProjection`: Select columns

**Join Order Enumeration**:
- Uses dynamic programming for optimal join order
- Cost model considers:
  - Cardinality estimates
  - Selectivity of predicates
  - Index availability

### Stage 4: Optimization

**Location**: `src/include/optimizer/optimizer.h:16-23`

Applies rule-based and cost-based optimizations:

```cpp
class Optimizer {
public:
    static void optimize(planner::LogicalPlan* plan,
                         main::ClientContext* context,
                         const planner::CardinalityEstimator& cardinalityEstimator);
};
```

**Optimization Passes**:

1. **Filter Push Down** (`filter_push_down_optimizer.h`)
   - Push predicates as close to data sources as possible
   - Reduces intermediate result sizes

2. **Projection Push Down** (`projection_push_down_optimizer.h`)
   - Eliminate unused columns early
   - Reduces I/O and memory

3. **Correlated Subquery Unnesting** (`correlated_subquery_unnest_solver.h`)
   - Transform correlated subqueries to joins
   - Enables better join order optimization

4. **Join Reordering** (`join_order/join_plan_solver.h`)
   - Find optimal join order using dynamic programming
   - Considers cardinality and cost

5. **Factorization Rewriting** (`factorization_rewriter.h`)
   - Exploit common sub-expressions
   - Reduce materialization

6. **Limit Push Down** (`limit_push_down_optimizer.h`)
   - Push LIMIT through operators when possible

7. **Top-K Optimization** (`top_k_optimizer.h`)
   - Special optimization for ORDER BY + LIMIT

### Stage 5: Execution

**Location**: `src/include/processor/operator/physical_operator.h:94-178`

Maps logical plan to **physical operators**:

```cpp
class PhysicalOperator {
public:
    // State initialization
    void initGlobalState(ExecutionContext* context);
    void initLocalState(ResultSet* resultSet, ExecutionContext* context);

    // Execution (Volcano-style iteration)
    bool getNextTuple(ExecutionContext* context);

    // Cleanup
    void finalize(ExecutionContext* context);
};
```

**Physical Operator Types** (121 operators total):

| Category | Operators |
|----------|-----------|
| **Scan** | `SCAN_NODE_TABLE`, `SCAN_REL_TABLE`, `PRIMARY_KEY_SCAN_NODE_TABLE` |
| **Join** | `HASH_JOIN_BUILD`, `HASH_JOIN_PROBE`, `INTERSECT`, `CROSS_PRODUCT` |
| **Graph** | `RECURSIVE_EXTEND`, `PATH_PROPERTY_PROBE` |
| **Aggregation** | `AGGREGATE`, `AGGREGATE_FINALIZE`, `AGGREGATE_SCAN` |
| **Set** | `UNION_ALL_SCAN`, `INTERSECT_BUILD` |
| **Data Flow** | `FILTER`, `PROJECTION`, `FLATTEN`, `UNWIND` |
| **Sort** | `ORDER_BY`, `ORDER_BY_MERGE`, `ORDER_BY_SCAN`, `TOP_K` |
| **Modification** | `INSERT`, `DELETE_`, `BATCH_INSERT`, `SET_PROPERTY` |
| **Utility** | `LIMIT`, `SKIP`, `RESULT_COLLECTOR` |

**Execution Model**:

```cpp
// QueryProcessor orchestrates execution
class QueryProcessor {
public:
    std::unique_ptr<main::QueryResult> execute(PhysicalPlan* plan,
                                                ExecutionContext* context);
private:
    std::unique_ptr<common::TaskScheduler> taskScheduler;
};
```

**Task-based Parallelism**:
- Physical plan decomposed into **Tasks**
- TaskScheduler distributes tasks across worker threads
- **Morsel-driven**: Data processed in batches (morsels) for parallelism

**Vectorized Execution**:
- Operators process data in **ValueVectors** (batches of ~2048 values)
- SIMD-friendly memory layout
- Reduces function call overhead

---

## Transaction Management

**Location**: `src/include/transaction/transaction.h:77-164`

Ryu implements **Multi-Version Concurrency Control (MVCC)** for isolation.

### Transaction Types

```cpp
enum class TransactionType : uint8_t {
    READ_ONLY,   // No modifications allowed
    WRITE,       // Can modify data
    CHECKPOINT,  // Special transaction for checkpointing
    DUMMY,       // Placeholder
    RECOVERY     // Used during WAL replay
};
```

### Transaction Structure

```cpp
class Transaction {
public:
    Transaction(main::ClientContext& ctx, TransactionType type,
                common::transaction_t ID, common::transaction_t startTS);

    // Transaction state
    common::transaction_t getID() const;
    common::transaction_t getStartTS() const;
    common::transaction_t getCommitTS() const;

    // Lifecycle
    void commit(storage::WAL* wal);
    void rollback(storage::WAL* wal);

    // Local storage (uncommitted changes)
    storage::LocalStorage* getLocalStorage() const;
    storage::LocalWAL& getLocalWAL() const;

    // Versioning
    bool isUnCommitted(common::table_id_t tableID, common::offset_t offset) const;

private:
    TransactionType type;
    common::transaction_t ID;          // Transaction ID
    common::transaction_t startTS;     // Start timestamp
    common::transaction_t commitTS;    // Commit timestamp

    std::unique_ptr<storage::LocalStorage> localStorage;
    std::unique_ptr<storage::UndoBuffer> undoBuffer;
    std::unique_ptr<storage::LocalWAL> localWAL;
};
```

### MVCC Implementation

**Version Visibility Rules**:

```cpp
bool NodeGroup::isVisible(const transaction::Transaction* txn,
                          common::row_idx_t rowIdx) const {
    // Check version information
    auto* versionInfo = getVersionInfo();

    // Row inserted by this transaction → visible
    if (versionInfo->isInserted(txn, rowIdx)) {
        return true;
    }

    // Row deleted by this transaction → not visible
    if (versionInfo->isDeleted(txn, rowIdx)) {
        return false;
    }

    // Check against start timestamp
    auto insertTS = versionInfo->getInsertTS(rowIdx);
    auto deleteTS = versionInfo->getDeleteTS(rowIdx);

    return (insertTS <= txn->getStartTS()) &&
           (deleteTS == INVALID_TS || deleteTS > txn->getStartTS());
}
```

**Key Concepts**:
- **Transaction ID**: Monotonically increasing, uniquely identifies transactions
- **Start Timestamp**: Determines snapshot visibility (which versions to see)
- **Commit Timestamp**: When transaction's changes become visible to others
- **Version Chains**: Each row has insert/delete timestamps

### LocalStorage

**Uncommitted Changes**:
```cpp
class LocalStorage {
public:
    // Track uncommitted inserts
    void insert(common::table_id_t tableID, const std::vector<ValueVector*>& vectors);

    // Track uncommitted updates
    void update(common::table_id_t tableID, common::offset_t offset, ...);

    // Track uncommitted deletes
    void delete_(common::table_id_t tableID, common::offset_t offset);

    // Scan uncommitted data
    void scan(common::table_id_t tableID, ...);
};
```

### UndoBuffer

**Rollback Information**:
```cpp
class UndoBuffer {
public:
    // Record insert for rollback
    void pushInsertInfo(common::node_group_idx_t nodeGroupIdx,
                        common::row_idx_t startRow,
                        common::row_idx_t numRows, ...);

    // Record delete for rollback
    void pushDeleteInfo(...);

    // Record update for rollback
    void pushVectorUpdateInfo(...);
};
```

### Transaction Lifecycle

```
┌─────────────┐
│   BEGIN     │
│ (allocate   │
│  txn ID,    │
│  start TS)  │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  EXECUTE    │
│ • Reads use │
│   snapshot  │
│ • Writes go │
│   to local  │
│   storage   │
└──────┬──────┘
       │
    ┌──┴──┐
    │     │
    ▼     ▼
┌────────┐ ┌──────��──┐
│ COMMIT │ │ROLLBACK │
│• Assign│ │• Discard│
│ commit │ │  local  │
│  TS    │ │ storage │
│• Write │ │• Apply  │
│  WAL   │ │  undo   │
│• Apply │ │  buffer │
│ changes│ └─────────┘
└────────┘
```

---

## Indexing System

### Primary Key Index

**Location**: `src/include/storage/index/hash_index.h:74-100`

Every node table has a **hash-based primary key index**:

```cpp
template<typename T>
class HashIndex final : public OnDiskHashIndex {
public:
    // Lookup: find offset given key
    bool lookup(const transaction::Transaction* txn, const Key& key,
                common::offset_t& result);

    // Insert: add key-value pair
    bool insert(const transaction::Transaction* txn, const Key& key,
                common::offset_t value);

    // Delete: mark key as deleted
    void delete_(const transaction::Transaction* txn, const Key& key);

    // Persistence
    bool checkpoint(PageAllocator& pageAllocator);
    void bulkReserve(uint64_t numValuesToAppend);

private:
    InMemHashIndex<T>* inMemIndex;        // In-memory during bulk load
    DiskArray<Slot<T>>* persistentSlots;  // Persistent storage
    LocalHashIndex<T>* localIndex;        // Per-transaction changes
};
```

**Lookup Process**:

1. **Check LocalHashIndex** (current transaction):
   - If key in local deletions → return false
   - If key in local insertions → return value

2. **Check Persistent Index**:
   - Hash key to slot
   - Linear probing for collision resolution
   - Return value if found

**Slot Structure**:
```cpp
template<typename T>
struct Slot {
    T key;
    common::offset_t offset;  // Node offset in table
    bool occupied;
};
```

### Secondary Indexes

**Location**: `src/include/catalog/catalog.h:130-160`

Support for various index types:

```cpp
class Catalog {
public:
    // Check if index exists
    bool containsIndex(const transaction::Transaction* txn,
                       common::table_id_t tableID,
                       const std::string& indexName) const;

    // Get index
    IndexCatalogEntry* getIndex(const transaction::Transaction* txn,
                                common::table_id_t tableID,
                                const std::string& indexName) const;

    // Create index
    void createIndex(transaction::Transaction* txn,
                     std::unique_ptr<CatalogEntry> indexEntry);

    // Drop index
    void dropIndex(transaction::Transaction* txn,
                   common::table_id_t tableID,
                   const std::string& indexName);
};
```

**Supported Index Types**:
- **Hash Index**: Default for primary keys
- **Full-Text Search (FTS)**: Via `fts` extension
- **Vector Index**: Via `vector` extension for similarity search
- **Custom Indexes**: Via extension framework

### Index Scan Optimization

Physical operators can leverage indexes:

```cpp
// Index lookup operator
class PhysicalIndexLookup : public PhysicalOperator {
public:
    bool getNextTuplesInternal(ExecutionContext* context) override {
        // Use index to find rows matching predicate
        index->lookup(key, offset);
        // Fetch full row from table
        table->lookup(offset, outputVectors);
    }
private:
    Index* index;
    NodeTable* table;
};
```

---

## Buffer Management

**Location**: `src/include/storage/buffer_manager/buffer_manager.h:187-296`

Ryu's buffer manager is inspired by **Umbra's vmcache** design, using virtual memory for efficient page management.

### Page States

Every page transitions through 4 states:

```
        pin(pX)          unpin(pX)
EVICTED ──────→ LOCKED ──────────→ MARKED
                  ▲                   │
                  │    pin(pX)        │
                  └───────────────────┘
                         │
               optimisticRead(pX)
                         ▼
                     UNLOCKED
```

1. **EVICTED**: Page not in memory
2. **LOCKED**: Page pinned, being modified (exclusive access)
3. **UNLOCKED**: Page in memory, recently accessed (optimistic reads allowed)
4. **MARKED**: Page in memory, candidate for eviction

### Buffer Manager Structure

```cpp
class BufferManager {
public:
    BufferManager(const std::string& databasePath,
                  const std::string& spillPath,
                  uint64_t bufferPoolSize,
                  uint64_t maxDBSize,
                  common::VirtualFileSystem* vfs,
                  bool readOnly);

    // Page operations (called via FileHandle)
    uint8_t* pin(FileHandle& fh, common::page_idx_t pageIdx,
                 PageReadPolicy policy = READ_PAGE);
    void unpin(FileHandle& fh, common::page_idx_t pageIdx);
    void optimisticRead(FileHandle& fh, common::page_idx_t pageIdx,
                        const std::function<void(uint8_t*)>& func);

    // Memory management
    uint64_t getUsedMemory() const;
    uint64_t getMemoryLimit() const;

private:
    bool reserve(uint64_t sizeToReserve);
    uint64_t evictPages();
    bool claimAFrame(FileHandle& fh, common::page_idx_t pageIdx, ...);

    std::atomic<uint64_t> bufferPoolSize;
    std::atomic<uint64_t> usedMemory;
    EvictionQueue evictionQueue;
    std::array<std::unique_ptr<VMRegion>, 2> vmRegions;  // Regular & temp pages
};
```

### VMRegion (Virtual Memory Region)

```cpp
class VMRegion {
public:
    VMRegion(uint64_t maxSize, common::PageSizeClass pageSizeClass);

    // Frame management
    uint8_t* getFrame(common::frame_idx_t frameIdx);
    void releaseFrame(common::frame_idx_t frameIdx);

    // Uses mmap for virtual memory mapping
private:
    uint8_t* region;  // mmap'd memory
    uint64_t maxSize;
    common::PageSizeClass pageSizeClass;  // REGULAR_PAGE or TEMP_PAGE
};
```

### Eviction Queue

**Circular buffer tracking eviction candidates**:

```cpp
class EvictionQueue {
public:
    static constexpr size_t BATCH_SIZE = 64;

    // Insert page as eviction candidate
    bool insert(uint32_t fileIndex, common::page_idx_t pageIndex);

    // Get next batch of candidates
    std::span<std::atomic<EvictionCandidate>, BATCH_SIZE> next();

    // Clear candidate after eviction
    void clear(std::atomic<EvictionCandidate>& candidate);

private:
    std::atomic<uint64_t> insertCursor;
    std::atomic<uint64_t> evictionCursor;
    std::unique_ptr<std::atomic<EvictionCandidate>[]> data;
};
```

### Optimistic Reads

**Lock-free reads for read-heavy workloads**:

```cpp
void BufferManager::optimisticRead(FileHandle& fh, common::page_idx_t pageIdx,
                                    const std::function<void(uint8_t*)>& func) {
    auto* pageState = fh.getPageState(pageIdx);

    while (true) {
        // Read version before accessing page
        auto versionBefore = pageState->getStateAndVersion();

        if (PageState::getState(versionBefore) == PageState::EVICTED) {
            // Must pin page first
            pin(fh, pageIdx);
            unpin(fh, pageIdx);
            continue;
        }

        // Access page data
        uint8_t* frame = getFrame(fh, pageIdx);
        func(frame);

        // Verify version unchanged
        auto versionAfter = pageState->getStateAndVersion();
        if (versionBefore == versionAfter) {
            return;  // Successful optimistic read
        }
        // Retry if version changed (page was modified)
    }
}
```

### Second-Chance Eviction

When evicting pages:

1. Try to evict **MARKED** pages first (preferred candidates)
2. If MARKED page is now **UNLOCKED** (recently accessed), give it a second chance:
   - Set it back to MARKED
   - Re-enqueue
3. This reduces thrashing for hot pages

### Memory Reclamation

Uses `madvise(MADV_DONTNEED)` on Linux to release physical memory:

```cpp
void releaseFrame(uint8_t* frame) {
    #ifdef __linux__
    madvise(frame, PAGE_SIZE, MADV_DONTNEED);
    #endif
}
```

This tells the OS it can reclaim the physical pages backing this virtual memory.

---

## Write-Ahead Logging

**Location**: `src/include/storage/wal/wal.h`

WAL ensures **durability** and **atomicity**.

### WAL Architecture

```
Transaction
    │
    ├─ LocalWAL (in-memory buffer)
    │     │
    │     ├─ INSERT records
    │     ├─ UPDATE records
    │     ├─ DELETE records
    │     └─ CATALOG records
    │
    ▼ (on commit)
    │
Global WAL File
    │
    ├─ WAL Header
    ├─ WAL Record 1
    ├─ WAL Record 2
    ├─ ...
    └─ WAL Record N
```

### WAL Record Structure

```cpp
class WALRecord {
public:
    enum class Type {
        INSERT,
        UPDATE,
        DELETE,
        TABLE_CREATE,
        TABLE_DROP,
        CATALOG_CHANGE,
        // ...
    };

    Type getType() const;
    void serialize(common::Serializer& ser);
    static std::unique_ptr<WALRecord> deserialize(common::Deserializer& deser);
};
```

### WAL Lifecycle

**Write Path**:

1. **Transaction executes modifications**
   - Changes buffered in `LocalStorage`
   - WAL records buffered in `LocalWAL`

2. **Transaction commits**
   ```cpp
   void Transaction::commit(storage::WAL* wal) {
       // 1. Assign commit timestamp
       commitTS = transactionManager->getNextCommitTS();

       // 2. Write WAL records to disk
       wal->append(localWAL->getRecords());
       wal->flush();  // Ensure durability

       // 3. Apply changes to storage
       localStorage->commit();
   }
   ```

3. **Checkpointing flushes data to disk**
   - In-memory changes written to table files
   - WAL truncated after successful checkpoint

**Recovery Path**:

```cpp
void StorageManager::recover(main::ClientContext& ctx,
                              bool throwOnWalReplayFailure,
                              bool enableChecksums) {
    // 1. Read WAL file
    auto walReader = WAL::openForReplay(databasePath, enableChecksums);

    // 2. Replay each record
    while (walReader->hasMoreRecords()) {
        auto record = walReader->readNextRecord();

        try {
            record->replay(ctx);  // Apply to storage
        } catch (...) {
            if (throwOnWalReplayFailure) {
                throw;
            }
            // Continue with partial recovery
        }
    }

    // 3. Mark recovery complete
    walReader->close();
}
```

### Checksums

**Optional corruption detection**:

```cpp
class ChecksumWriter {
public:
    void write(const uint8_t* data, uint64_t length) {
        // Compute CRC32
        uint32_t checksum = crc32(data, length);

        // Write data + checksum
        file->write(data, length);
        file->write(&checksum, sizeof(checksum));
    }
};

class ChecksumReader {
public:
    void read(uint8_t* data, uint64_t length) {
        // Read data + checksum
        file->read(data, length);
        uint32_t expectedChecksum;
        file->read(&expectedChecksum, sizeof(expectedChecksum));

        // Verify
        uint32_t actualChecksum = crc32(data, length);
        if (actualChecksum != expectedChecksum) {
            throw Exception("WAL corruption detected");
        }
    }
};
```

---

## Memory Management

**Location**: `src/include/storage/buffer_manager/memory_manager.h`

### MemoryManager

Manages **temporary in-memory allocations** (not backed by disk files):

```cpp
class MemoryManager {
public:
    MemoryManager(BufferManager& bufferManager);

    // Allocate temporary memory buffer
    std::unique_ptr<MemoryBuffer> allocateBuffer(uint64_t size);

    // Get memory statistics
    uint64_t getUsedMemory() const;
    uint64_t getAvailableMemory() const;

private:
    BufferManager& bufferManager;
    FileHandle* inMemFileHandle;  // Virtual file for temp allocations
};
```

### Spilling to Disk

When memory is tight, Ryu can **spill** intermediate results:

```cpp
class Spiller {
public:
    // Write data to spill file
    void spill(const FactorizedTable& table);

    // Read spilled data back
    std::unique_ptr<FactorizedTable> readSpilledData();

private:
    std::string spillPath;
    FileHandle* spillFile;
};
```

**Use Cases**:
- Large hash join build sides
- Large sort operations
- Aggregations with many groups

### FactorizedTable

**Efficient in-memory table representation**:

```cpp
class FactorizedTable {
public:
    // Append tuple
    void append(const std::vector<common::ValueVector*>& vectors);

    // Scan tuples
    void scan(const std::vector<common::ValueVector*>& outputVectors,
              uint64_t& tupleIdx);

    // Compute memory usage
    uint64_t getNumTuples() const;
    uint64_t getMemoryUsage() const;

    // Spilling support
    void serialize(common::Serializer& ser);
    static std::unique_ptr<FactorizedTable> deserialize(common::Deserializer& deser);
};
```

---

## Extension System

**Location**: `src/include/extension/extension.h`

### Extension Types

Ryu supports 5 extension points:

```cpp
// 1. Transformer Extension: Query transformation before parsing
class TransformerExtension {
public:
    virtual std::string transform(const std::string& query) = 0;
};

// 2. Binder Extension: Custom binding logic
class BinderExtension {
public:
    virtual std::unique_ptr<BoundStatement> bind(
        const parser::Statement& stmt,
        Binder& binder
    ) = 0;
};

// 3. Planner Extension: Custom planning logic
class PlannerExtension {
public:
    virtual std::unique_ptr<LogicalPlan> plan(
        const BoundStatement& stmt,
        Planner& planner
    ) = 0;
};

// 4. Mapper Extension: Custom physical operator mapping
class MapperExtension {
public:
    virtual std::unique_ptr<PhysicalOperator> map(
        const LogicalOperator* logicalOp,
        PlanMapper& mapper
    ) = 0;
};

// 5. Catalog Extension: Custom catalog entries
class CatalogExtension {
public:
    virtual void registerCatalogEntries(Catalog& catalog) = 0;
};
```

### Built-in Extensions

Four extensions are **pre-installed** (since v0.11.3):

1. **`algo`**: Graph algorithms
   - Shortest path
   - PageRank
   - Connected components
   - Community detection

2. **`fts`**: Full-text search
   - Inverted indexes
   - Tokenization
   - Ranking (BM25)

3. **`json`**: JSON support
   - JSON parsing
   - JSON path queries
   - JSON aggregation

4. **`vector`**: Vector embeddings
   - Vector similarity search
   - HNSW (Hierarchical Navigable Small World) index
   - Cosine/Euclidean distance

### Extension Loading

```cpp
// Install extension (download from server)
INSTALL extension_name FROM 'http://localhost:8080/';

// Load extension (into memory)
LOAD EXTENSION extension_name;
```

**Docker-based Extension Server**:
```bash
docker pull ghcr.io/ryugraph/extension-repo:latest
docker run -d -p 8080:80 ghcr.io/ryugraph/extension-repo:latest
```

---

## API & Language Bindings

**Location**: `tools/`

### C API

**Location**: `src/c_api/`

Foundation for all language bindings:

```c
// Database
ryu_database* ryu_database_init(const char* path, ryu_system_config config);
void ryu_database_destroy(ryu_database* db);

// Connection
ryu_connection* ryu_connection_init(ryu_database* db);
void ryu_connection_destroy(ryu_connection* conn);

// Query execution
ryu_query_result* ryu_connection_query(ryu_connection* conn, const char* query);
bool ryu_query_result_has_next(ryu_query_result* result);
ryu_flat_tuple* ryu_query_result_get_next(ryu_query_result* result);

// Value access
ryu_value* ryu_flat_tuple_get_value(ryu_flat_tuple* tuple, uint64_t idx);
int64_t ryu_value_get_int64(ryu_value* value);
const char* ryu_value_get_string(ryu_value* value);
```

### Python API

**Location**: `tools/python_api/`

**PyBind11-based bindings**:

```python
import ryudb

# Create database
db = ryudb.Database("test.db")

# Create connection
conn = ryudb.Connection(db)

# Execute query
result = conn.execute("MATCH (p:Person) RETURN p.name, p.age")

# Iterate results
while result.has_next():
    row = result.get_next()
    print(row[0], row[1])

# Prepared statements
stmt = conn.prepare("MATCH (p:Person) WHERE p.age = $age RETURN p.name")
result = conn.execute(stmt, {"age": 25})
```

### Rust API

**Location**: `tools/rust_api/`

```rust
use ryudb::{Database, SystemConfig};

// Create database
let db = Database::new("test.db", SystemConfig::default())?;

// Create connection
let conn = db.connect()?;

// Execute query
let mut result = conn.query("MATCH (p:Person) RETURN p.name, p.age")?;

// Iterate results
while let Some(row) = result.next()? {
    let name: String = row.get(0)?;
    let age: i64 = row.get(1)?;
    println!("{}: {}", name, age);
}
```

### JavaScript/Node.js API

**Location**: `tools/nodejs_api/`

```javascript
const ryu = require('ryudb');

// Create database
const db = new ryu.Database('test.db');

// Create connection
const conn = new ryu.Connection(db);

// Execute query
const result = conn.query('MATCH (p:Person) RETURN p.name, p.age');

// Iterate results
while (result.hasNext()) {
    const row = result.getNext();
    console.log(row[0], row[1]);
}
```

### WebAssembly

**Location**: `tools/wasm/`

Runs Ryu in the browser:

```javascript
import RyuDB from 'ryudb-wasm';

// Initialize
const ryu = await RyuDB.init();

// Create in-memory database
const db = await ryu.createDatabase(':memory:');

// Execute queries
const result = await db.query('MATCH (p:Person) RETURN p.name');
console.log(result.toJSON());
```

**Features**:
- In-memory databases
- Persistent storage via IndexedDB (browser)
- File system support (Node.js)

### Java API

**Location**: `tools/java_api/`

**JNI-based bindings**:

```java
import com.ryu.Database;
import com.ryu.Connection;
import com.ryu.QueryResult;

// Create database
Database db = new Database("test.db");

// Create connection
Connection conn = new Connection(db);

// Execute query
QueryResult result = conn.query("MATCH (p:Person) RETURN p.name, p.age");

// Iterate results
while (result.hasNext()) {
    Object[] row = result.next();
    System.out.println(row[0] + ": " + row[1]);
}
```

---

## Performance Optimizations

### 1. Vectorized Execution

**Batch Processing**:
- Operators process **ValueVectors** (batches of ~2048 values)
- Reduces virtual function call overhead
- Enables SIMD optimizations

**Example**:
```cpp
// Traditional tuple-at-a-time
for (auto i = 0; i < numTuples; i++) {
    outputVector[i] = inputVector1[i] + inputVector2[i];  // 1 call per tuple
}

// Vectorized
vectorAdd(inputVector1, inputVector2, outputVector, numTuples);  // 1 call for all
```

### 2. Columnar Storage

**Benefits**:
- **Better compression**: Similar values stored together
- **Cache-friendly**: Sequential access patterns
- **Projection pushdown**: Read only needed columns
- **SIMD-friendly**: Operate on contiguous data

**Layout**:
```
Row-oriented (traditional):
[ID:1, Name:"Alice", Age:25] [ID:2, Name:"Bob", Age:30] ...

Column-oriented (Ryu):
IDs:   [1, 2, 3, ...]
Names: ["Alice", "Bob", "Carol", ...]
Ages:  [25, 30, 28, ...]
```

### 3. CSR Adjacency Lists

**Fast Graph Traversal**:
- O(1) lookup for node's neighbors
- Sequential memory access for relationship properties
- Exploits graph locality

### 4. Factorized Execution

**Avoid Materialization**:
```cypher
MATCH (a:Person)-[:KNOWS]->(b:Person)
WHERE a.age > 25
RETURN a.name, b.name
```

**Traditional**:
1. Scan all Person nodes → materialize
2. Extend via KNOWS → materialize joined results
3. Filter by age → materialize filtered results
4. Project columns → final result

**Factorized**:
1. Scan Person nodes (a) with age > 25 → factorized table
2. For each 'a', extend via KNOWS → append to factorization
3. Project without full materialization

### 5. Parallel Query Execution

**Morsel-Driven Parallelism**:
- Data divided into **morsels** (chunks)
- Worker threads process morsels independently
- Load balancing via work-stealing

**Example**:
```
Table with 1,000,000 rows, 4 threads

Morsel size = 10,000 rows

Thread 1: rows [0, 10k)
Thread 2: rows [10k, 20k)
Thread 3: rows [20k, 30k)
Thread 4: rows [30k, 40k)

... continues until all rows processed
```

### 6. Join Optimizations

**Hash Join**:
- Build hash table on smaller side
- Probe with larger side
- Bloom filters for early filtering

**Worst-Case Optimal Joins** (WCOJ):
- For complex multi-way joins (common in graph queries)
- Intersect adjacency lists
- Avoid Cartesian products

### 7. Compression

**Per-Column Compression**:
- Dictionary encoding (strings)
- Run-length encoding (RLE)
- Frame-of-reference encoding (integers)
- Lightweight compression for fast decompression

### 8. Index-Aware Planning

**Index Selection**:
- Planner considers available indexes
- Transforms table scans to index lookups when beneficial
- Cost model accounts for index access cost

### 9. Adaptive Radix Tree (ART)

Used internally for:
- String dictionaries
- Prefix searches
- Range queries

**Benefits**:
- Compact memory footprint
- Cache-friendly
- Fast point and range queries

---

## Key Innovations

### 1. Embedded Graph Database with Columnar Storage

**Unique Combination**:
- Most graph databases (Neo4j, ArangoDB) use **row-oriented** storage
- Ryu uses **columnar** storage for analytical performance
- Combines graph semantics with data warehouse efficiency

### 2. CSR Format for Relationship Storage

**Compressed Sparse Row (CSR)**:
- Standard in sparse matrix libraries
- Applied to graph adjacency lists
- Enables O(1) neighbor lookup
- Sequential access for relationship properties

**Innovation**: Hybrid sequential/explicit encoding:
- Sequential: `(startRow, length)` for 2 values
- Explicit: `[row1, row2, ..., rowN]` for N values

### 3. Factorized Query Processing

**Exploit Redundancy**:
- Avoid materializing intermediate results
- Represent results symbolically when possible
- Significant memory savings for star-schema queries

**Example**:
```cypher
MATCH (p:Person)-[:LIVES_IN]->(c:City)
RETURN p.name, c.name
```

Traditional: Materialize all (Person, City) pairs
Factorized: Store Person list + City list + join keys

### 4. VMCache-Inspired Buffer Management

**Optimistic Reads**:
- Lock-free reads for read-heavy workloads
- Version checking prevents stale reads
- Inspired by Umbra's vmcache design

**Second-Chance Eviction**:
- Hot pages get second chance before eviction
- Reduces thrashing

### 5. Multi-Language Support via C API

**Stable ABI**:
- C API provides stable interface
- All language bindings built on C API
- Easier to maintain than per-language bindings

### 6. WebAssembly Support

**Browser Deployment**:
- Full database in the browser
- No server required for small datasets
- Persistent storage via IndexedDB

### 7. Extension Framework

**Dynamic Functionality**:
- Load features on-demand
- Third-party extensions possible
- Reduces core binary size

### 8. MVCC with Optimistic Locking

**Hybrid Approach**:
- Read transactions: Lock-free optimistic reads
- Write transactions: Optimistic concurrency control
- Serializability without heavy locking

### 9. Adaptive Query Execution

**Runtime Adaptivity**:
- Task scheduler adapts to available threads
- Morsel size adjusts based on workload
- Spilling activates under memory pressure

### 10. Worst-Case Optimal Joins

**Graph Query Optimization**:
- Traditional binary joins inefficient for complex graph patterns
- WCOJ intersects multiple adjacency lists simultaneously
- Provably optimal for cyclic queries

---

## Summary

Ryu is a **sophisticated embedded graph database** that brings together:

### Architecture Highlights

- **~102,000 lines** of well-structured C++20 code
- **Modular design**: Clear separation of concerns (catalog, storage, execution, transaction)
- **Hierarchical storage**: Tables → NodeGroups → ColumnChunks → Segments
- **CSR adjacency lists**: O(1) graph traversal
- **MVCC transactions**: Serializable isolation without heavy locking

### Performance Characteristics

- **Columnar + Vectorized**: Analytical query performance
- **Parallel execution**: Multi-threaded query processing
- **Factorized processing**: Reduced materialization overhead
- **Index-aware planning**: Intelligent use of indexes
- **VMCache buffer management**: Efficient memory utilization

### Unique Strengths

1. **Embeddable**: No client-server overhead, runs in-process
2. **Analytical focus**: Columnar storage unlike most graph databases
3. **Graph semantics**: CSR structures, recursive operators
4. **ACID guarantees**: Full transactional support via MVCC + WAL
5. **Multi-platform**: Linux, macOS, Windows, WebAssembly
6. **Multi-language**: Python, Rust, Java, Node.js, C API
7. **Extensible**: Plugin architecture for custom features

### Comparison to Similar Systems

| Feature | Ryu | Neo4j | DuckDB |
|---------|-----|-------|--------|
| **Storage** | Columnar | Row-oriented | Columnar |
| **Deployment** | Embedded | Client-Server | Embedded |
| **Query Language** | Cypher | Cypher | SQL |
| **Graph Support** | Native | Native | Extensions |
| **OLAP Performance** | High | Medium | High |
| **OLTP Performance** | Medium | High | Medium |

Ryu occupies a **unique niche**: the analytical graph database that's embeddable, combining the best of graph databases (native graph support, Cypher) with modern analytical databases (columnar storage, vectorization).

---

## References

### Source Code Locations

- **Database Core**: `src/include/main/database.h`, `src/include/main/connection.h`
- **Storage Layer**: `src/include/storage/table/`, `src/include/storage/buffer_manager/`
- **Query Processing**: `src/include/processor/`, `src/include/planner/`, `src/include/optimizer/`
- **Transactions**: `src/include/transaction/`
- **Catalog**: `src/include/catalog/`
- **Indexes**: `src/include/storage/index/`
- **Extensions**: `src/include/extension/`
- **Language Bindings**: `tools/python_api/`, `tools/rust_api/`, `tools/nodejs_api/`, etc.

### External Resources

- **Documentation**: https://ryugraph.github.io/docs
- **Blog**: https://ryugraph.github.io/blog
- **GitHub**: https://github.com/predictable-labs/ryugraph
- **License**: MIT (see `LICENSE` file)

---

*This document was generated through comprehensive analysis of the Ryu codebase (v0.11.2.2) on 2025-10-21.*
