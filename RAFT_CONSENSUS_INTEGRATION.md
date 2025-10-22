# Raft Consensus Integration with Backup Feature

## Executive Summary

This document analyzes how the proposed **zero-downtime backup feature** integrates with **Raft consensus** for distributed Ryu deployments. Currently, Ryu is a **single-node embedded database** without built-in replication. This document outlines:

1. Current state: Ryu as an embedded database
2. How the backup feature works in single-node mode
3. Raft consensus fundamentals
4. How backup and Raft consensus interact
5. Design considerations for distributed Ryu with Raft
6. Roadmap for adding Raft support

---

## Table of Contents

1. [Current State: Ryu as Embedded Database](#current-state-ryu-as-embedded-database)
2. [Raft Consensus Fundamentals](#raft-consensus-fundamentals)
3. [Backup Feature in Single-Node Context](#backup-feature-in-single-node-context)
4. [Raft + Backup Integration Scenarios](#raft--backup-integration-scenarios)
5. [Design for Distributed Ryu with Raft](#design-for-distributed-ryu-with-raft)
6. [Implementation Roadmap](#implementation-roadmap)
7. [Comparison with Existing Systems](#comparison-with-existing-systems)

---

## Current State: Ryu as Embedded Database

### Architecture Analysis

Based on codebase analysis, **Ryu does NOT currently have**:
- ❌ Distributed replication
- ❌ Raft consensus implementation
- ❌ Cluster coordination
- ❌ Multi-node deployment
- ❌ Leader election
- ❌ Log replication across nodes

**Evidence from codebase**:
```bash
# No Raft/consensus/replication code found
$ grep -r "raft\|consensus\|replication\|distributed" src/include --include="*.h"
# (no results)

$ grep -r "cluster\|replica\|leader\|follower" src/include --include="*.h"
# (no results)
```

### Current Deployment Model

```
┌─────────────────────────────────────┐
│      Application Process             │
│  ┌───────────────────────────────┐  │
│  │     Ryu Database              │  │
│  │  (Embedded, Single-Node)      │  │
│  │                               │  │
│  │  • MVCC Transactions          │  │
│  │  • WAL for Durability         │  │
│  │  • Checkpoint Mechanism       │  │
│  │  • No Replication             │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
           │
           ▼
     Local Disk
```

**Key Characteristics**:
- **Embedded**: Runs in same process as application
- **Single-node**: No distributed state
- **ACID**: Via MVCC + WAL (single node)
- **High availability**: Not supported (no replicas)

---

## Raft Consensus Fundamentals

### What is Raft?

**Raft** is a consensus algorithm that enables a cluster of nodes to agree on a replicated state machine, providing:
- **Leader election**: One node is the leader, others are followers
- **Log replication**: Leader replicates log entries to followers
- **Safety**: Guarantees consistency even with node failures
- **Availability**: System remains available as long as majority (quorum) is alive

### Raft Architecture

```
┌────────────────────────────────────────────────────────────┐
│                    Raft Cluster                             │
├────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌─────────────┐      ┌─────────────┐      ┌───────────┐ │
│   │   Node 1    │      │   Node 2    │      │  Node 3   │ │
│   │  (Leader)   │◄────►│ (Follower)  │◄────►│(Follower) │ │
│   └─────────────┘      └─────────────┘      └───────────┘ │
│         │                     │                    │        │
│         │  Replicate Logs     │                    │        │
│         ├────────────────────►│                    │        │
│         ├───────────────────────────────────────►  │        │
│         │                     │                    │        │
│         │  Acknowledge        │                    │        │
│         │◄────────────────────┤                    │        │
│         │◄───────────────────────────────────────┤│        │
│         │                     │                    │        │
│    ┌────▼────┐          ┌────▼────┐         ┌────▼────┐   │
│    │  State  │          │  State  │         │  State  │   │
│    │ Machine │          │ Machine │         │ Machine │   │
│    │  (DB)   │          │  (DB)   │         │  (DB)   │   │
│    └─────────┘          └─────────┘         └─────────┘   │
└────────────────────────────────────────────────────────────┘
```

### Raft Key Concepts

#### 1. Roles
- **Leader**: Handles all client requests, replicates log to followers
- **Follower**: Passively receives log entries from leader
- **Candidate**: Node seeking to become leader during election

#### 2. Log Replication
```
Client Write Request
        │
        ▼
    ┌───────┐
    │Leader │
    └───┬───┘
        │ 1. Append to local log
        │ 2. Replicate to followers
        ├──────────────┐
        │              │
        ▼              ▼
   ┌─────────┐    ┌─────────┐
   │Follower1│    │Follower2│
   └────┬────┘    └────┬────┘
        │              │
        │ 3. Acknowledge
        └──────┬───────┘
               ▼
         ┌─────────┐
         │ Leader  │
         └────┬────┘
              │ 4. Commit when majority acks
              │ 5. Apply to state machine
              ▼
        Client Response
```

#### 3. Consensus Guarantees
- **Election Safety**: At most one leader per term
- **Leader Append-Only**: Leader never overwrites/deletes log entries
- **Log Matching**: If two logs contain entry with same index/term, all preceding entries are identical
- **Leader Completeness**: If log entry committed in a term, it will be in leader's log for all future terms
- **State Machine Safety**: If a node has applied a log entry at index i, no other node will apply a different entry at index i

---

## Backup Feature in Single-Node Context

### Current Backup Design (Single-Node)

The proposed backup feature for single-node Ryu:

```
Single Ryu Instance
        │
        ├─ Main Thread: Serve queries
        │
        └─ Backup Thread: Copy database files
                │
                ├─ Snapshot TS: Record transaction ID
                ├─ Copy pages with COW
                └─ Verify and finalize backup
```

**Key Properties**:
- ✅ Zero downtime (background thread)
- ✅ Transactional consistency (snapshot isolation)
- ✅ Point-in-time backup
- ✅ No distributed coordination needed

**Works well for**: Single-node deployments, development, small applications

**Limitations for distributed systems**:
- ❌ No cross-replica consistency
- ❌ No cluster-wide snapshot
- ❌ Cannot backup "distributed state"
- ❌ No handling of leader changes

---

## Raft + Backup Integration Scenarios

### Scenario 1: Backup on Leader Node Only

**Strategy**: Only the Raft leader performs backups.

```
Raft Cluster (3 nodes)

┌─────────────┐      ┌─────────────┐      ┌─────────────┐
│   Leader    │      │  Follower1  │      │  Follower2  │
│             │      │             │      │             │
│ ┌─────────┐ │      │ ┌─────────┐ │      │ ┌─────────┐ │
│ │ Backup  │ │      │ │   No    │ │      │ │   No    │ │
│ │ Thread  │ │      │ │ Backup  │ │      │ │ Backup  │ │
│ └────┬────┘ │      │ └─────────┘ │      │ └─────────┘ │
│      │      │      │             │      │             │
│      ▼      │      │             │      │             │
│  Backup     │      │             │      │             │
│  Files      │      │             │      │             │
└─────────────┘      └─────────────┘      └─────────────┘
```

**Pros**:
- ✅ Simple: No coordination needed
- ✅ Backup is consistent with committed state (leader has all committed entries)
- ✅ No extra network traffic for backup coordination

**Cons**:
- ❌ Leader may be overloaded (queries + replication + backup)
- ❌ If leader fails during backup, backup is lost
- ❌ Doesn't protect against leader disk corruption

**When to use**: Small clusters, low backup frequency, leader has spare resources

### Scenario 2: Backup on Follower Node

**Strategy**: Offload backup to a follower to avoid leader overhead.

```
Raft Cluster (3 nodes)

┌─────────────┐      ┌─────────────┐      ┌─────────────┐
│   Leader    │      │  Follower1  │      │  Follower2  │
│             │      │             │      │             │
│             │      │ ┌─────────┐ │      │             │
│  (Serves    │      │ │ Backup  │ │      │  (Normal    │
│   Queries)  │      │ │ Thread  │ │      │   Replica)  │
│             │      │ └────┬────┘ │      │             │
│             │      │      │      │      │             │
│             │      │      ▼      │      │             │
│             │      │  Backup     │      │             │
│             │      │  Files      │      │             │
└─────────────┘      └─────────────┘      └─────────────┘
```

**Pros**:
- ✅ Offloads work from leader
- ✅ Leader performance unaffected
- ✅ Backup still consistent (follower has committed entries)

**Cons**:
- ❌ Follower may lag behind leader (backup slightly stale)
- ❌ Need to handle case where follower is promoted to leader mid-backup
- ❌ Follower disk I/O affects replication performance

**When to use**: High query load on leader, followers have spare I/O capacity

### Scenario 3: Coordinated Cluster-Wide Backup

**Strategy**: All nodes participate in creating a consistent cluster-wide snapshot.

```
Raft Cluster (3 nodes)

User: BACKUP CLUSTER TO '/backup'
        │
        ▼
   ┌─────────┐
   │ Leader  │ ◄── Receives backup command
   └────┬────┘
        │ 1. Choose snapshot point (Raft log index)
        │ 2. Send "prepare backup" to followers
        │
        ├───────────────┬────────────────┐
        ▼               ▼                ▼
   ┌─────────┐    ┌─────────┐     ┌─────────┐
   │ Leader  │    │Follower1│     │Follower2│
   │         │    │         │     │         │
   │ Backup  │    │ Backup  │     │ Backup  │
   │ Thread  │    │ Thread  │     │ Thread  │
   └────┬────┘    └────┬────┘     └────┬────┘
        │              │               │
        │ 3. Wait for all nodes to finish
        └──────────────┴───────────────┘
                       │
                       ▼
              Cluster Backup Complete
```

**Pros**:
- ✅ True cluster-wide consistency
- ✅ All replicas backed up simultaneously
- ✅ Protects against any single node failure
- ✅ Can verify backup across multiple nodes

**Cons**:
- ❌ Complex coordination
- ❌ Requires Raft log index synchronization
- ❌ Higher network overhead
- ❌ Backup slowest node determines completion time

**When to use**: Mission-critical systems, disaster recovery requirements, compliance

---

## Design for Distributed Ryu with Raft

### Prerequisites

To add Raft consensus to Ryu, we need to implement:

#### 1. Raft Library Integration

**Options**:
- **etcd/raft** (Go library, used by etcd, Cockroach DB)
- **NuRaft** (C++ library, fork of Apache Kudu Raft)
- **braft** (C++ library, used by Apache Kudu)
- **Custom implementation** (full control, high effort)

**Recommendation**: **NuRaft** (C++, production-ready, Apache 2.0 license)

#### 2. State Machine Interface

Ryu needs to implement Raft state machine:

```cpp
class RyuStateMachine : public raft::StateMachine {
public:
    // Apply committed log entry to database
    void apply(const raft::LogEntry& entry) override {
        // Deserialize entry
        auto walRecord = deserializeWALRecord(entry.data);

        // Apply to local database (same as WAL replay)
        walRecord->apply(clientContext);
    }

    // Create snapshot of current state
    void createSnapshot(raft::SnapshotWriter& writer) override {
        // This is where our backup feature integrates!
        backupManager->createSnapshot(writer);
    }

    // Load snapshot
    void loadSnapshot(raft::SnapshotReader& reader) override {
        // This is where our restore feature integrates!
        backupManager->restoreSnapshot(reader);
    }
};
```

#### 3. Raft-Aware Backup Manager

```cpp
namespace ryu::storage {

class RaftBackupManager : public BackupManager {
public:
    RaftBackupManager(main::ClientContext& context,
                      raft::RaftNode* raftNode);

    // Backup strategies
    void backupLeaderOnly(const std::string& backupPath);
    void backupFollower(const std::string& backupPath);
    void backupClusterCoordinated(const std::string& backupPath);

    // Raft snapshot integration
    void createRaftSnapshot(raft::SnapshotWriter& writer);
    void loadRaftSnapshot(raft::SnapshotReader& reader);

private:
    // Raft coordination
    uint64_t getCurrentRaftLogIndex();
    void waitForLogIndexReplicated(uint64_t logIndex);
    void coordinateClusterBackup(uint64_t snapshotLogIndex);

    raft::RaftNode* raftNode;
};

} // namespace ryu::storage
```

### Enhanced Backup with Raft Coordination

#### Strategy: Coordinated Cluster-Wide Backup

```cpp
void RaftBackupManager::backupClusterCoordinated(const std::string& backupPath) {
    // Only leader can initiate cluster backup
    if (!raftNode->isLeader()) {
        throw BackupException("Only leader can initiate cluster backup");
    }

    // 1. Choose snapshot point (current Raft log index)
    auto snapshotLogIndex = raftNode->getLastCommittedIndex();

    // 2. Create a special "backup barrier" entry in Raft log
    //    This ensures all nodes reach the same log point before backup
    auto barrierEntry = createBackupBarrierEntry(snapshotLogIndex);
    raftNode->appendEntry(barrierEntry);

    // 3. Wait for backup barrier to be replicated and committed
    waitForLogIndexReplicated(snapshotLogIndex);

    // 4. Send "prepare backup" RPC to all followers
    auto backupId = generateBackupId();
    sendPrepareBackupRPC(backupId, snapshotLogIndex, backupPath);

    // 5. Start local backup on leader
    BackupMetadata metadata;
    metadata.snapshotTS = transactionManager->getCurrentTS();
    metadata.raftLogIndex = snapshotLogIndex;
    metadata.backupId = backupId;

    // Use existing backup mechanism (from BACKUP_FEATURE_DESIGN.md)
    BackupManager::startBackup(backupPath, metadata);

    // 6. Wait for all nodes to complete backup
    waitForClusterBackupCompletion(backupId);

    // 7. Verify backup across all nodes
    verifyClusterBackup(backupId);
}
```

### Raft Log + WAL Integration

**Challenge**: Raft already has a log, Ryu has WAL. How do they relate?

**Solution**: Raft log **replaces** WAL for replication, but WAL is still used locally for crash recovery.

```
┌───────────────────────────────────────────────────────┐
│                   Raft Node                            │
│                                                        │
│  Client Write                                          │
│       │                                                │
│       ▼                                                │
│  ┌─────────────────┐                                  │
│  │  Raft Log       │  Replicated to followers         │
│  │  (Distributed)  ├─────────────────────────────►    │
│  └────────┬────────┘                                  │
│           │ Apply when committed                      │
│           ▼                                            │
│  ┌─────────────────┐                                  │
│  │  Local WAL      │  For crash recovery              │
│  │  (Single-node)  │                                  │
│  └────────┬────────┘                                  │
│           │                                            │
│           ▼                                            │
│  ┌─────────────────┐                                  │
│  │  Database       │                                  │
│  │  (Ryu)          │                                  │
│  └─────────────────┘                                  │
└───────────────────────────────────────────────────────┘
```

**Workflow**:
1. **Write**: Client writes to leader
2. **Raft log**: Leader appends to Raft log, replicates to followers
3. **Commit**: Once majority acks, entry is committed
4. **Apply**: Committed entry applied to Ryu database
5. **Local WAL**: Before applying, write to local WAL (for crash recovery)
6. **Checkpoint**: Periodically checkpoint, truncate local WAL (Raft log persists)

### Backup with Raft Log Integration

```cpp
void RaftBackupManager::createRaftSnapshot(raft::SnapshotWriter& writer) {
    auto snapshotLogIndex = raftNode->getLastAppliedIndex();

    // 1. Record snapshot metadata
    BackupMetadata metadata;
    metadata.raftLogIndex = snapshotLogIndex;
    metadata.snapshotTS = transactionManager->getCurrentTS();

    // 2. Use existing backup mechanism to copy database state
    BackupManager::copyDatabaseState(writer, metadata);

    // 3. Raft will automatically truncate log entries before snapshotLogIndex
    //    (they're now captured in snapshot)
}

void RaftBackupManager::loadRaftSnapshot(raft::SnapshotReader& reader) {
    // 1. Read snapshot metadata
    auto metadata = BackupMetadata::deserialize(reader);

    // 2. Use existing restore mechanism
    BackupManager::restoreDatabaseState(reader, metadata);

    // 3. Update Raft state to reflect snapshot
    raftNode->setLastAppliedIndex(metadata.raftLogIndex);
}
```

---

## Implementation Roadmap

### Phase 0: Current State (Completed)
- ✅ Single-node Ryu
- ✅ MVCC transactions
- ✅ WAL for durability
- ✅ Checkpoint mechanism
- 🚧 Zero-downtime backup (proposed in BACKUP_FEATURE_DESIGN.md)

### Phase 1: Single-Node Backup (6 weeks)
**Goal**: Implement backup feature for single-node Ryu (as per BACKUP_FEATURE_DESIGN.md)

- Week 1-2: Core backup infrastructure
- Week 3-4: Copy-on-write mechanism
- Week 5-6: WAL handling, verification, testing

**Deliverable**: Production-ready backup for single-node deployments

### Phase 2: Raft Integration Foundation (8-10 weeks)
**Goal**: Add Raft consensus to Ryu (multi-node replication)

#### 2.1 Raft Library Integration (2 weeks)
- Integrate NuRaft library
- Create CMake build configuration
- Basic Raft node setup

#### 2.2 State Machine Implementation (3 weeks)
- Implement `RyuStateMachine`
- Map Ryu transactions to Raft log entries
- Apply committed entries to database

#### 2.3 Network Layer (2 weeks)
- Implement RPC for Raft communication
- Node discovery and cluster membership
- Health checks and monitoring

#### 2.4 Transaction Routing (2 weeks)
- Route writes to leader
- Leader forwards writes to Raft
- Follower redirection

#### 2.5 Testing (1 week)
- Multi-node cluster tests
- Failover tests
- Consistency tests

**Deliverable**: Basic Raft-replicated Ryu cluster (3+ nodes)

### Phase 3: Raft-Aware Backup (4-6 weeks)
**Goal**: Extend backup to work with Raft clusters

#### 3.1 Leader-Only Backup (1 week)
- Modify `BackupManager` to check Raft role
- Only allow backup on leader
- Test backup during normal operation

#### 3.2 Follower Backup (2 weeks)
- Implement backup on follower
- Handle follower-to-leader promotion during backup
- Test with leader changes

#### 3.3 Coordinated Cluster Backup (2-3 weeks)
- Implement backup barrier in Raft log
- Coordinate backup across all nodes
- Verify consistency across replicas

#### 3.4 Raft Snapshot Integration (1 week)
- Integrate backup with Raft snapshot mechanism
- Use backup for log compaction
- Test snapshot load/save

**Deliverable**: Production-ready backup for Raft clusters

### Phase 4: Advanced Features (Post-MVP)
- Incremental backups in Raft clusters
- Cross-datacenter replication
- Read replicas (followers serve reads)
- Automatic backup scheduling
- Cloud storage integration

---

## Comparison with Existing Systems

### How Other Distributed Databases Handle Backup

#### 1. **CockroachDB** (Uses Raft)

**Backup Strategy**:
- **Cluster-wide distributed backup**
- Each node backs up its range of data
- Coordinator ensures consistency across nodes
- Supports incremental backups

**Similarities with our design**:
- ✅ Uses Raft for replication
- ✅ Backup doesn't block writes
- ✅ Point-in-time consistency

**Differences**:
- CockroachDB is distributed-first (we're adding distribution to embedded DB)
- Range-based partitioning (we have full replication for now)

#### 2. **etcd** (Uses Raft)

**Backup Strategy**:
- **Leader-only snapshot**
- Snapshot is a point-in-time copy of entire key-value store
- Integrated with Raft snapshot mechanism
- Simple, effective for small databases

**Similarities**:
- ✅ Raft-based replication
- ✅ Snapshot = backup
- ✅ Leader handles snapshot creation

**Differences**:
- etcd is small (MB-GB), Ryu targets larger databases (GB-TB)
- etcd snapshots are synchronous (blocks writes briefly)
- Our backup is async, zero-downtime

#### 3. **TiDB** (Uses Raft via TiKV)

**Backup Strategy**:
- **BR (Backup & Restore)** tool
- Distributed backup across TiKV nodes
- Each TiKV node backs up its regions
- Coordinator assembles full backup

**Similarities**:
- ✅ Raft-based storage layer
- ✅ Distributed backup coordination
- ✅ Zero-downtime backups

**Differences**:
- TiDB has separate storage layer (TiKV) and compute layer (TiDB)
- We're embedded (no separate layers)
- TiDB is range-partitioned, we're fully replicated

#### 4. **PostgreSQL with Patroni** (Uses etcd/Consul for consensus)

**Backup Strategy**:
- **pg_basebackup** on primary or standby
- Continuous archiving (WAL shipping)
- Point-in-time recovery via WAL replay

**Similarities**:
- ✅ Backup from primary or standby
- ✅ WAL-based recovery
- ✅ Zero-downtime backups

**Differences**:
- Postgres uses streaming replication (not Raft)
- Patroni uses etcd/Consul for leader election only (not data replication)
- We use Raft for both consensus and data replication

---

## Backup Feature + Raft: Integration Summary

### Answer: **Yes, the backup feature can work with Raft, with modifications**

#### What Works As-Is (Single-Node)
✅ Snapshot isolation mechanism
✅ Copy-on-write for modified pages
✅ WAL snapshot copying
✅ Backup verification
✅ Restore from backup

#### What Needs Modification (Raft Cluster)

##### Minor Changes Needed:
1. **Role awareness**: Check if node is leader/follower before allowing backup
2. **Raft log index**: Include Raft log index in backup metadata
3. **Transaction ID mapping**: Map Raft log index to transaction IDs

##### Major Changes Needed (for coordinated cluster backup):
1. **Backup coordination**: Leader coordinates backup across all nodes
2. **Backup barrier**: Special Raft log entry to synchronize backup point
3. **Cross-node verification**: Verify backup consistency across all nodes
4. **Raft snapshot integration**: Use backup mechanism for Raft snapshots

### Recommended Approach

#### For v1 (Single-Node):
✅ **Implement backup as designed in BACKUP_FEATURE_DESIGN.md**
- No Raft, no distributed concerns
- Production-ready backup for embedded use cases
- Foundation for future Raft integration

#### For v2 (Raft Support):
🔄 **Add Raft consensus, simple leader-only backup**
- Integrate NuRaft library
- Implement basic replication
- Backup only on leader
- Good enough for most use cases

#### For v3 (Advanced):
🚀 **Add coordinated cluster backup**
- Distributed backup coordination
- Backup across all nodes
- Cross-node verification
- Mission-critical deployments

---

## Conclusion

### Key Takeaways

1. **Current Ryu**: Single-node embedded database, no Raft/replication
2. **Proposed backup**: Works perfectly for single-node Ryu (zero-downtime, consistent)
3. **Raft integration**: Possible, requires additional coordination layer
4. **Recommendation**: Implement single-node backup first, add Raft later

### Decision Points

| Feature | Single-Node Backup | Raft + Leader Backup | Raft + Coordinated Backup |
|---------|-------------------|---------------------|--------------------------|
| **Complexity** | Low | Medium | High |
| **Consistency** | ✅ Point-in-time | ✅ Point-in-time | ✅ Cluster-wide |
| **Availability** | ❌ Single point of failure | ✅ Replicated | ✅✅ Highly available |
| **Performance Impact** | Low (<10%) | Low (<10%) | Medium (<20%) |
| **Recommended For** | Embedded, small apps | Most production use | Mission-critical |

### Next Steps

1. ✅ **Approve and implement** single-node backup (BACKUP_FEATURE_DESIGN.md)
2. 🔄 **Evaluate** Raft integration needs based on user requirements
3. 🚀 **Plan** Raft implementation if distributed deployment is needed

---

*End of Raft Consensus Integration Document*
