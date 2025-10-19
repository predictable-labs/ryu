#!/bin/bash
# Test shell locally using the same environment as CI
# This mimics what GitHub Actions does for the shell tests

set -e

echo "Testing shell locally..."
echo ""

# Check if we need to build first
if [ ! -f "build/release/tools/shell/ryu" ]; then
    echo "Shell binary not found. Building first..."
    echo ""

    # Create build directory if it doesn't exist
    mkdir -p build/release
    cd build/release

    # Configure with CMake
    echo "=== Configuring with CMake ==="
    cmake -DCMAKE_BUILD_TYPE=Release ../..

    echo ""
    echo "=== Building shell ==="
    make shell -j$(sysctl -n hw.ncpu)

    cd ../..
    echo ""
fi

echo "=== Running shell tests ==="
cd tools/shell

# Install test dependencies if needed
if ! python3 -c "import pytest" 2>/dev/null; then
    echo "Installing pytest..."
    pip3 install pytest
fi

# Run the specific tests that were failing
echo ""
echo "Running shell mode tests..."
python3 -m pytest test/test_shell_flags.py::test_mode -v
python3 -m pytest test/test_shell_commands.py::test_set_mode -v

echo ""
echo "✅ Shell tests completed!"
