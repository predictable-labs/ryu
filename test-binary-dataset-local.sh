#!/bin/bash
# Test binary dataset generation locally with MinIO
# This mimics what GitHub Actions does for the generate-binary-datasets job

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
USE_MINIO=${USE_MINIO:-true}

echo "Testing binary dataset generation locally..."
echo ""

# Start MinIO if requested
MINIO_STARTED=false
if [ "$USE_MINIO" = "true" ]; then
    echo "=== Setting up MinIO ==="

    # Check if MinIO is already running
    if ! curl -s http://localhost:9000/minio/health/live > /dev/null 2>&1; then
        echo "Starting MinIO..."
        "$SCRIPT_DIR/scripts/minio/start-minio.sh"
        MINIO_STARTED=true
    else
        echo "MinIO is already running"
    fi

    # Setup s3cmd configuration
    echo ""
    echo "Configuring s3cmd for MinIO..."
    "$SCRIPT_DIR/scripts/minio/setup-s3cmd.sh"

    echo ""
fi

# Check if we need to build first
if [ ! -f "build/release/tools/shell/ryu" ]; then
    echo "Release build not found. Building first..."
    echo ""
    make release
    echo ""
fi

echo "=== Testing binary-demo dataset generation ==="
bash scripts/generate_binary_demo.sh

if [ -d "dataset/binary-demo" ]; then
    echo "✅ binary-demo dataset generated successfully"
    echo "   Location: dataset/binary-demo"
    ls -lh dataset/binary-demo/
else
    echo "❌ binary-demo dataset generation failed"
    exit 1
fi

echo ""
echo "=== Testing tinysnb dataset generation ==="
bash scripts/generate_binary_tinysnb.sh

if [ -d "tinysnb" ]; then
    echo "✅ tinysnb dataset generated successfully"
    echo "   Location: tinysnb"
    ls -lh tinysnb/

    if [ "$USE_MINIO" = "true" ]; then
        echo ""
        echo "=== Testing S3 upload to MinIO ==="

        # Create version file
        version_current="$(python3 benchmark/version.py)"
        echo "$version_current" > tinysnb/version.txt

        # Upload to MinIO
        echo "Uploading tinysnb to MinIO bucket ryu-test..."
        s3cmd sync ./tinysnb s3://ryu-test/

        echo ""
        echo "Verifying upload..."
        s3cmd ls s3://ryu-test/tinysnb/

        echo ""
        echo "Testing download from MinIO..."
        s3cmd get --force s3://ryu-test/tinysnb/version.txt version_test.txt

        if [ -f "version_test.txt" ]; then
            downloaded_version="$(cat version_test.txt)"
            if [ "$version_current" = "$downloaded_version" ]; then
                echo "✅ S3 upload/download to MinIO successful!"
                echo "   Version: $version_current"
            else
                echo "❌ Version mismatch: expected $version_current, got $downloaded_version"
            fi
            rm version_test.txt
        else
            echo "❌ Failed to download version file from MinIO"
        fi
    fi

    rm -rf tinysnb  # Clean up
else
    echo "❌ tinysnb dataset generation failed"
    exit 1
fi

echo ""
echo "✅ All binary dataset generation tests passed!"
echo ""

if [ "$USE_MINIO" = "true" ]; then
    echo "MinIO Console: http://localhost:9001 (minioadmin/minioadmin)"
    echo "View buckets with: s3cmd ls"
    echo ""
    if [ "$MINIO_STARTED" = "true" ]; then
        echo "To stop MinIO: ./scripts/minio/stop-minio.sh"
    fi
else
    echo "Note: MinIO was not used. Set USE_MINIO=true to test S3 uploads"
fi
echo ""
