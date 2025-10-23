#!/bin/bash

################################################################################
# Self-Hosted GitHub Actions Runner - Complete Setup Script
################################################################################
#
# This script installs ALL dependencies required to run the CI workflow
# (ci-workflow.yml) on a self-hosted Linux runner.
#
# Compatible with: Ubuntu 20.04, 22.04, 24.04
#
# Usage:
#   ./setup-self-hosted-runner-complete.sh
#
################################################################################

set -e  # Exit on error

echo "================================================================================"
echo "  Self-Hosted GitHub Actions Runner - Complete Setup"
echo "================================================================================"
echo ""
echo "This script will install all dependencies needed for ci-workflow.yml"
echo ""

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    echo "ERROR: Please do not run this script as root/sudo."
    echo "The script will prompt for sudo password when needed."
    exit 1
fi

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    OS_VERSION=$VERSION_ID
    echo "Detected OS: $PRETTY_NAME"
else
    echo "ERROR: Cannot detect OS version"
    exit 1
fi

# Verify it's Ubuntu
if [ "$OS" != "ubuntu" ]; then
    echo "WARNING: This script is designed for Ubuntu. Detected: $OS"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo ""
echo "================================================================================"
echo "Step 1: Update Package Lists"
echo "================================================================================"
sudo apt-get update

echo ""
echo "================================================================================"
echo "Step 2: Install Essential Build Tools"
echo "================================================================================"
echo "Installing: build-essential, cmake, ninja-build, git, wget, curl..."
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    wget \
    curl \
    unzip \
    zip

echo ""
echo "================================================================================"
echo "Step 3: Install Clang Toolchain"
echo "================================================================================"
echo "Installing: clang, clangd, clang-tidy, clang-format-18..."
sudo apt-get install -y \
    clang \
    clangd \
    clang-tidy \
    clang-format-18

echo ""
echo "================================================================================"
echo "Step 4: Install Java (OpenJDK 21)"
echo "================================================================================"
echo "Installing: openjdk-21-jdk, openjdk-21-jre..."
sudo apt-get install -y \
    openjdk-21-jdk \
    openjdk-21-jre

# Set JAVA_HOME if not already set
if [ -z "$JAVA_HOME" ]; then
    JAVA_HOME_PATH=$(update-alternatives --query java | grep Value: | cut -d' ' -f2 | sed 's|/bin/java||')
    echo "export JAVA_HOME=$JAVA_HOME_PATH" >> ~/.bashrc
    echo "export PATH=\$JAVA_HOME/bin:\$PATH" >> ~/.bashrc
    export JAVA_HOME=$JAVA_HOME_PATH
    export PATH=$JAVA_HOME/bin:$PATH
fi

echo ""
echo "================================================================================"
echo "Step 5: Install Library Dependencies"
echo "================================================================================"
echo "Installing: libtool, libltdl-dev, libedit-dev, libssl-dev..."
sudo apt-get install -y \
    libtool \
    libltdl-dev \
    libedit-dev \
    libssl-dev

echo ""
echo "================================================================================"
echo "Step 6: Install Testing and Coverage Tools"
echo "================================================================================"
echo "Installing: lcov, s3cmd..."
sudo apt-get install -y \
    lcov \
    s3cmd

echo ""
echo "================================================================================"
echo "Step 7: Install Python 3 and pip"
echo "================================================================================"
sudo apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    python3-venv

echo ""
echo "================================================================================"
echo "Step 8: Install Python Packages"
echo "================================================================================"
echo "Installing: rangehttpserver, requests, pytest, pexpect..."

# For Ubuntu 24.04+, use pipx or --break-system-packages
if command -v pipx &> /dev/null; then
    echo "Using pipx to install Python packages..."
    pipx install rangehttpserver || true
    # For libraries, we need to use pip with --break-system-packages or install via apt
    pip3 install --user --break-system-packages --upgrade \
        rangehttpserver \
        requests \
        pytest \
        pexpect
else
    # Try with --break-system-packages flag (Ubuntu 24.04+)
    pip3 install --user --break-system-packages --upgrade \
        rangehttpserver \
        requests \
        pytest \
        pexpect || \
    # Fallback to regular install for older Ubuntu
    pip3 install --user --upgrade \
        rangehttpserver \
        requests \
        pytest \
        pexpect
fi

echo ""
echo "================================================================================"
echo "Step 9: Install Node.js 20 LTS"
echo "================================================================================"
if ! command -v node &> /dev/null || [ "$(node -v | cut -d'v' -f2 | cut -d'.' -f1)" -lt 20 ]; then
    echo "Installing Node.js 20 LTS..."
    curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
    sudo apt-get install -y nodejs
else
    NODE_VERSION=$(node -v)
    NODE_MAJOR=$(echo $NODE_VERSION | cut -d'v' -f2 | cut -d'.' -f1)
    if [ "$NODE_MAJOR" -ge 20 ]; then
        echo "Node.js $NODE_VERSION is already installed and meets requirements (>= 20)"
    else
        echo "Upgrading Node.js from $NODE_VERSION to version 20..."
        curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
        sudo apt-get install -y nodejs
    fi
fi

echo ""
echo "================================================================================"
echo "Step 10: Install Rust and Cargo"
echo "================================================================================"
if ! command -v rustc &> /dev/null; then
    echo "Installing Rust using rustup..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable

    # Source cargo env for current session
    source "$HOME/.cargo/env"

    # Ensure it's in bashrc for future sessions
    if ! grep -q 'source $HOME/.cargo/env' ~/.bashrc; then
        echo '' >> ~/.bashrc
        echo '# Rust/Cargo environment' >> ~/.bashrc
        echo 'source $HOME/.cargo/env' >> ~/.bashrc
    fi

    echo "Rust installed: $(rustc --version)"
    echo "Cargo installed: $(cargo --version)"
else
    echo "Rust is already installed: $(rustc --version)"
    echo "Cargo version: $(cargo --version)"
    echo "Updating Rust to latest stable..."
    rustup update stable
    rustup default stable
fi

# Verify cargo is in PATH
if ! command -v cargo &> /dev/null; then
    echo "WARNING: cargo not found in PATH after installation"
    echo "Attempting to source cargo env..."
    source "$HOME/.cargo/env"
    if command -v cargo &> /dev/null; then
        echo "✓ cargo is now available"
    else
        echo "ERROR: cargo installation may have failed"
    fi
fi

echo ""
echo "================================================================================"
echo "Step 11: Install Docker (for MinIO and other containers)"
echo "================================================================================"
if ! command -v docker &> /dev/null; then
    echo "Installing Docker..."
    # Add Docker's official GPG key
    sudo apt-get install -y ca-certificates gnupg
    sudo install -m 0755 -d /etc/apt/keyrings
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
    sudo chmod a+r /etc/apt/keyrings/docker.gpg

    # Add Docker repository
    echo \
      "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
      $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
      sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

    # Install Docker
    sudo apt-get update
    sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

    # Add current user to docker group
    sudo usermod -aG docker $USER
    echo "NOTE: You need to log out and log back in for Docker group membership to take effect"
else
    echo "Docker is already installed: $(docker --version)"
fi

echo ""
echo "================================================================================"
echo "Step 12: Install DuckDB v1.1.3 System-Wide"
echo "================================================================================"
if [ -f /usr/local/lib/libduckdb.so ]; then
    echo "DuckDB is already installed at /usr/local/lib/libduckdb.so"
    read -p "Do you want to reinstall DuckDB? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Skipping DuckDB installation"
    else
        echo "Reinstalling DuckDB v1.1.3..."
        rm -rf /tmp/duckdb-src
        git clone --depth 1 --branch v1.1.3 https://github.com/duckdb/duckdb.git /tmp/duckdb-src
        cd /tmp/duckdb-src
        mkdir build && cd build
        CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_UNITTESTS=0 -DBUILD_SHELL=0 ..
        make -j$(nproc) duckdb
        sudo cp src/libduckdb.so /usr/local/lib/
        sudo cp ../src/include/duckdb.h /usr/local/include/
        sudo cp ../src/include/duckdb.hpp /usr/local/include/
        sudo cp -r ../src/include/duckdb /usr/local/include/
        sudo ldconfig
        cd ~ && rm -rf /tmp/duckdb-src
        echo "DuckDB reinstalled successfully"
    fi
else
    echo "Building and installing DuckDB v1.1.3 from source..."
    rm -rf /tmp/duckdb-src
    git clone --depth 1 --branch v1.1.3 https://github.com/duckdb/duckdb.git /tmp/duckdb-src
    cd /tmp/duckdb-src
    mkdir build && cd build
    CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_UNITTESTS=0 -DBUILD_SHELL=0 ..
    make -j$(nproc) duckdb
    sudo cp src/libduckdb.so /usr/local/lib/
    sudo cp ../src/include/duckdb.h /usr/local/include/
    sudo cp ../src/include/duckdb.hpp /usr/local/include/
    sudo cp -r ../src/include/duckdb /usr/local/include/
    sudo ldconfig
    cd ~ && rm -rf /tmp/duckdb-src
    echo "DuckDB installed successfully"
fi

# Return to original directory
cd "$HOME"

echo ""
echo "================================================================================"
echo "Step 13: Verify All Installations"
echo "================================================================================"
echo ""

MISSING_DEPS=0

echo "Build Tools:"
echo "  gcc: $(gcc --version | head -n1 || echo 'MISSING')"
echo "  g++: $(g++ --version | head -n1 || echo 'MISSING')"
echo "  make: $(make --version | head -n1 || echo 'MISSING')"
echo "  cmake: $(cmake --version | head -n1 || echo 'MISSING')"
echo "  ninja: $(ninja --version || echo 'MISSING')"
echo ""

echo "Clang Toolchain:"
echo "  clang: $(clang --version | head -n1 || echo 'MISSING')"
echo "  clang++: $(clang++ --version | head -n1 || echo 'MISSING')"
echo "  clangd: $(clangd --version | head -n1 || echo 'MISSING')"
echo "  clang-tidy: $(clang-tidy --version | head -n1 || echo 'MISSING')"
echo "  clang-format-18: $(clang-format-18 --version | head -n1 || echo 'MISSING')"
echo ""

echo "Java:"
echo "  java: $(java -version 2>&1 | head -n1 || echo 'MISSING')"
echo "  javac: $(javac -version 2>&1 || echo 'MISSING')"
echo "  JAVA_HOME: ${JAVA_HOME:-'NOT SET'}"
echo ""

echo "Node.js:"
echo "  node: $(node --version || echo 'MISSING')"
echo "  npm: $(npm --version || echo 'MISSING')"
echo ""

echo "Python:"
echo "  python3: $(python3 --version || echo 'MISSING')"
echo "  pip3: $(pip3 --version || echo 'MISSING')"
echo ""

echo "Rust & Cargo:"
# Source cargo env in case it was just installed
source "$HOME/.cargo/env" 2>/dev/null || true
if command -v rustc &> /dev/null; then
    echo "  ✓ rustc: $(rustc --version)"
else
    echo "  ✗ rustc: MISSING"
    MISSING_DEPS=1
fi

if command -v cargo &> /dev/null; then
    echo "  ✓ cargo: $(cargo --version)"
else
    echo "  ✗ cargo: MISSING"
    MISSING_DEPS=1
fi

if command -v rustup &> /dev/null; then
    echo "  ✓ rustup: $(rustup --version)"
else
    echo "  ✗ rustup: MISSING"
fi
echo ""

echo "Docker:"
echo "  docker: $(docker --version 2>/dev/null || echo 'MISSING (or need to login again)')"
echo ""

echo "DuckDB:"
if [ -f /usr/local/lib/libduckdb.so ]; then
    echo "  ✓ libduckdb.so installed at /usr/local/lib/libduckdb.so"
    echo "  ✓ Headers installed at /usr/local/include/duckdb.h"
else
    echo "  ✗ DuckDB not found"
    MISSING_DEPS=1
fi
echo ""

echo "Coverage & Testing Tools:"
echo "  lcov: $(lcov --version 2>&1 | head -n1 || echo 'MISSING')"
echo "  s3cmd: $(s3cmd --version 2>&1 | head -n1 || echo 'MISSING')"
echo ""

echo ""
echo "================================================================================"
echo "Step 14: Check Passwordless Sudo Configuration"
echo "================================================================================"
echo ""

if sudo -n true 2>/dev/null; then
    echo "✓ Passwordless sudo is already configured"
else
    echo "✗ Passwordless sudo is NOT configured"
    echo ""
    echo "IMPORTANT: GitHub Actions requires passwordless sudo for self-hosted runners."
    echo ""
    echo "To configure passwordless sudo:"
    echo "  1. Run: sudo visudo"
    echo "  2. Add this line at the end (replace '$(whoami)' with your username):"
    echo "     $(whoami) ALL=(ALL) NOPASSWD: ALL"
    echo ""
    echo "Or for better security, only allow specific commands:"
    echo "     $(whoami) ALL=(ALL) NOPASSWD: /usr/bin/apt-get, /usr/bin/cp, /usr/bin/mkdir, /usr/bin/chmod, /sbin/ldconfig"
    echo ""
fi

echo ""
echo "================================================================================"
echo "Installation Summary"
echo "================================================================================"
echo ""

if [ $MISSING_DEPS -eq 0 ]; then
    echo "✓ All dependencies installed successfully!"
else
    echo "⚠ Some dependencies may be missing. Please review the output above."
fi

echo ""
echo "Next Steps:"
echo "  1. If passwordless sudo is not configured, configure it as shown above"
echo "  2. If Docker was just installed, log out and log back in for group membership"
echo "  3. If Rust was just installed, reload your shell to update environment variables:"
echo "     source ~/.bashrc"
echo "     # Or simply log out and log back in"
echo "  4. Configure and start your GitHub Actions runner"
echo "  5. Test the runner by triggering a workflow"
echo ""
echo "Optional - Install Python dev dependencies for the project:"
echo "  cd /path/to/ryugraph"
echo "  pip3 install --user -r tools/python_api/requirements_dev.txt"
echo ""
echo "Environment Variables to Verify:"
echo "  - JAVA_HOME should point to Java installation"
echo "  - cargo should be in PATH (from ~/.cargo/env)"
echo "  - You may need to run: source ~/.bashrc"
echo ""
echo "================================================================================"
echo "Setup Complete!"
echo "================================================================================"
echo ""
