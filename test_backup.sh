#!/bin/bash

# Ryu Database Backup Test Script
# This script provides a simple way to manually test backup functionality

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test directories
DB_PATH="/tmp/ryu_backup_test"
BACKUP_PATH="/tmp/ryu_backup"
RESTORE_PATH="/tmp/ryu_restore"

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_step() {
    echo -e "${GREEN}► $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

cleanup() {
    print_step "Cleaning up test directories..."
    rm -rf "$DB_PATH" "$BACKUP_PATH" "$RESTORE_PATH"
}

create_test_database() {
    print_header "Step 1: Creating Test Database"

    rm -rf "$DB_PATH"

    print_step "Creating database at $DB_PATH"

    # Create database and populate with test data using ryu shell
    ./build/release/tools/shell/ryu "$DB_PATH" <<EOF
CREATE NODE TABLE Person(name STRING, age INT64, PRIMARY KEY(name));
CREATE NODE TABLE City(name STRING, population INT64, PRIMARY KEY(name));
CREATE REL TABLE LivesIn(FROM Person TO City);

CREATE (:Person {name: 'Alice', age: 30});
CREATE (:Person {name: 'Bob', age: 25});
CREATE (:Person {name: 'Carol', age: 35});
CREATE (:City {name: 'New York', population: 8000000});
CREATE (:City {name: 'London', population: 9000000});

MATCH (p:Person {name: 'Alice'}), (c:City {name: 'New York'}) CREATE (p)-[:LivesIn]->(c);
MATCH (p:Person {name: 'Bob'}), (c:City {name: 'London'}) CREATE (p)-[:LivesIn]->(c);

MATCH (p:Person) RETURN p.name, p.age;
EOF

    print_success "Test database created"
}

query_database() {
    local db_path=$1
    local label=$2

    print_header "$label"

    print_step "Querying people..."
    ./build/release/tools/shell/ryu "$db_path" <<EOF
MATCH (p:Person) RETURN p.name, p.age ORDER BY p.name;
EOF

    print_step "Querying cities..."
    ./build/release/tools/shell/ryu "$db_path" <<EOF
MATCH (c:City) RETURN c.name, c.population ORDER BY c.name;
EOF
}

test_basic_backup() {
    print_header "Test 1: Basic Backup Test"

    create_test_database

    print_header "Step 2: Query Original Database"
    query_database "$DB_PATH" "Original Database Data"

    print_header "Step 3: Backup Database"
    print_warning "Note: Backup API needs to be exposed via shell or C API"
    print_warning "For now, demonstrating manual file copy as placeholder"

    rm -rf "$BACKUP_PATH"
    mkdir -p "$BACKUP_PATH"

    # Manual backup approach (copy files)
    print_step "Copying database files to $BACKUP_PATH..."
    cp -r "$DB_PATH" "$BACKUP_PATH/db_copy"

    print_success "Backup completed (manual copy)"

    print_header "Step 4: Verify Backup Size"
    echo "Original database size:"
    du -sh "$DB_PATH"
    echo "Backup size:"
    du -sh "$BACKUP_PATH"

    print_header "Step 5: Restore from Backup"
    rm -rf "$RESTORE_PATH"
    cp -r "$BACKUP_PATH/db_copy" "$RESTORE_PATH"

    print_success "Database restored to $RESTORE_PATH"

    print_header "Step 6: Verify Restored Database"
    query_database "$RESTORE_PATH" "Restored Database Data"

    print_success "Basic backup test completed!"
}

test_backup_files() {
    print_header "Inspecting Database Files"

    if [ -d "$DB_PATH" ]; then
        print_step "Database directory contents:"
        ls -lh "$DB_PATH"
        echo ""

        print_step "Database file sizes:"
        du -h "$DB_PATH"/*
    else
        print_error "Database directory not found: $DB_PATH"
    fi
}

show_usage() {
    cat <<EOF
Usage: $0 [command]

Commands:
    test            Run basic backup test
    create          Create test database only
    query           Query existing test database
    backup          Backup test database (manual copy)
    restore         Restore from backup
    inspect         Inspect database files
    cleanup         Remove all test directories
    help            Show this help message

Examples:
    $0 test         # Run full backup test
    $0 create       # Create test database
    $0 query        # Query test database
    $0 cleanup      # Clean up test files

Test Directories:
    Database:  $DB_PATH
    Backup:    $BACKUP_PATH
    Restored:  $RESTORE_PATH
EOF
}

# Main script
case "${1:-test}" in
    test)
        test_basic_backup
        ;;
    create)
        create_test_database
        ;;
    query)
        if [ ! -d "$DB_PATH" ]; then
            print_error "Database not found. Run '$0 create' first."
            exit 1
        fi
        query_database "$DB_PATH" "Test Database"
        ;;
    backup)
        if [ ! -d "$DB_PATH" ]; then
            print_error "Database not found. Run '$0 create' first."
            exit 1
        fi
        rm -rf "$BACKUP_PATH"
        mkdir -p "$BACKUP_PATH"
        cp -r "$DB_PATH" "$BACKUP_PATH/db_copy"
        print_success "Backup created at $BACKUP_PATH"
        ;;
    restore)
        if [ ! -d "$BACKUP_PATH/db_copy" ]; then
            print_error "Backup not found. Run '$0 backup' first."
            exit 1
        fi
        rm -rf "$RESTORE_PATH"
        cp -r "$BACKUP_PATH/db_copy" "$RESTORE_PATH"
        print_success "Database restored to $RESTORE_PATH"
        query_database "$RESTORE_PATH" "Restored Database"
        ;;
    inspect)
        test_backup_files
        ;;
    cleanup)
        cleanup
        print_success "Cleanup completed"
        ;;
    help)
        show_usage
        ;;
    *)
        print_error "Unknown command: $1"
        echo ""
        show_usage
        exit 1
        ;;
esac
