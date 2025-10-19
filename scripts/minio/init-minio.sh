#!/bin/bash
# Initialize MinIO for local S3 testing
# This script installs MinIO client and sets up buckets

set -e

echo "Initializing MinIO for local S3 testing..."
echo ""

# Check if MinIO is running
if ! curl -s http://localhost:9000/minio/health/live > /dev/null 2>&1; then
    echo "Error: MinIO is not running at localhost:9000"
    echo "Please start MinIO first using: ./scripts/minio/start-minio.sh"
    exit 1
fi

echo "MinIO is running!"
echo ""

# Install MinIO client if not already installed
if ! command -v mc &> /dev/null; then
    echo "Installing MinIO client (mc)..."

    # Detect OS
    OS="$(uname -s)"
    case "${OS}" in
        Linux*)
            curl -O https://dl.min.io/client/mc/release/linux-amd64/mc
            chmod +x mc
            sudo mv mc /usr/local/bin/
            ;;
        Darwin*)
            brew install minio/stable/mc || {
                curl -O https://dl.min.io/client/mc/release/darwin-amd64/mc
                chmod +x mc
                sudo mv mc /usr/local/bin/
            }
            ;;
        *)
            echo "Unsupported OS: ${OS}"
            exit 1
            ;;
    esac

    echo "MinIO client installed successfully!"
else
    echo "MinIO client already installed"
fi

echo ""
echo "Configuring MinIO client..."

# Configure mc to connect to local MinIO
mc alias set local http://localhost:9000 minioadmin minioadmin

echo ""
echo "Creating buckets..."

# Create buckets
mc mb local/ryu-test --ignore-existing
mc mb local/ryugraph-test --ignore-existing

# Set download policy for buckets
mc anonymous set download local/ryu-test
mc anonymous set download local/ryugraph-test

echo ""
echo "MinIO initialization complete!"
echo ""
echo "Available buckets:"
mc ls local/

echo ""
echo "MinIO Console: http://localhost:9001"
echo "Username: minioadmin"
echo "Password: minioadmin"
echo ""
