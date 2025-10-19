#!/bin/bash
# Test CI steps locally using Docker
# This mimics the GitHub Actions CI workflow

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "🧪 Testing RyuGraph CI steps locally..."
echo ""

# Function to run tests in Ubuntu container (like CI)
test_in_ubuntu() {
    echo "📦 Testing in Ubuntu container (like GitHub Actions)..."

    docker run --rm \
      -v "$SCRIPT_DIR:/workspace" \
      -w /workspace \
      -e NUM_THREADS=6 \
      -e GEN=Ninja \
      -e CC=clang \
      -e CXX=clang++ \
      ubuntu:latest \
      bash -c '
        set -e

        echo "=== Installing dependencies ==="
        apt-get update -qq
        apt-get install -y -qq \
          build-essential \
          cmake \
          ninja-build \
          clang \
          wget \
          unzip \
          curl \
          git \
          > /dev/null

        echo ""
        echo "=== Installing DuckDB ==="
        wget -q https://github.com/duckdb/duckdb/releases/download/v1.1.3/libduckdb-linux-amd64.zip
        unzip -q libduckdb-linux-amd64.zip -d /tmp/duckdb
        cp /tmp/duckdb/duckdb.h /usr/local/include/
        cp /tmp/duckdb/duckdb.hpp /usr/local/include/
        cp /tmp/duckdb/libduckdb.so /usr/local/lib/
        ldconfig
        rm libduckdb-linux-amd64.zip

        echo ""
        echo "=== Configuring build ==="
        cmake -B build/release \
          -DCMAKE_BUILD_TYPE=Release \
          -G Ninja \
          -DBUILD_EXTENSIONS="duckdb" \
          .

        echo ""
        echo "=== Building ==="
        cmake --build build/release --parallel $NUM_THREADS

        echo ""
        echo "✅ Build completed successfully!"
      '
}

# Function to test Rust API
test_rust_api() {
    echo ""
    echo "🦀 Testing Rust API..."

    docker run --rm \
      -v "$SCRIPT_DIR:/workspace" \
      -w /workspace/tools/rust_api \
      -e CARGO_BUILD_JOBS=6 \
      rust:1.81 \
      bash -c '
        set -e

        echo "=== Cargo version ==="
        cargo --version

        echo ""
        echo "=== Checking Cargo.toml and Cargo.lock ==="
        echo "Package name in Cargo.toml:"
        grep "^name = " Cargo.toml

        echo ""
        echo "Package name in Cargo.lock:"
        grep "^name = \"ryugraph\"" Cargo.lock || echo "❌ ryugraph not found in Cargo.lock"

        echo ""
        echo "=== Running cargo test with --locked flag ==="
        cargo test --release --locked --all-features

        echo ""
        echo "✅ Rust tests passed!"
      '
}

# Run tests
echo "Choose test to run:"
echo "1) Full Ubuntu build test (slower, ~10 min)"
echo "2) Rust API test only (faster, ~2 min)"
echo "3) Both"
echo ""
read -p "Enter choice [1-3]: " choice

case $choice in
    1)
        test_in_ubuntu
        ;;
    2)
        test_rust_api
        ;;
    3)
        test_in_ubuntu
        test_rust_api
        ;;
    *)
        echo "Invalid choice. Running Rust test only..."
        test_rust_api
        ;;
esac

echo ""
echo "🎉 All tests completed successfully!"
