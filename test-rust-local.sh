#!/bin/bash
# Test Rust API build locally using Docker
# This mimics what GitHub Actions does for the Rust tests

set -e

echo "Testing Rust API build in Docker container..."
echo ""

docker run --rm \
  -v "$(pwd):/workspace" \
  -w /workspace/tools/rust_api \
  -e CARGO_BUILD_JOBS=6 \
  rust:1.81 \
  bash -c "
    set -e

    echo '=== Installing build dependencies ==='
    apt-get update -qq > /dev/null 2>&1
    apt-get install -y -qq cmake build-essential > /dev/null 2>&1

    echo '=== Cargo version ==='
    cargo --version

    echo ''
    echo '=== Checking package name ==='
    echo 'Package name in Cargo.toml:'
    grep '^name = ' Cargo.toml
    echo ''
    echo 'Package name in Cargo.lock:'
    grep '^name = \"ryugraph\"' Cargo.lock | head -1 || echo '❌ ryugraph not found in Cargo.lock'

    echo ''
    echo '=== Running cargo test with --locked flag (like CI) ==='
    cargo test --release --locked --all-features

    echo ''
    echo '✅ Rust tests passed!'
  "

echo ""
echo "✅ All tests completed successfully!"
