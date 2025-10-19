#!/bin/bash
# Configure s3cmd to work with local MinIO

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
S3CFG_TEMPLATE="$SCRIPT_DIR/.s3cfg.template"
S3CFG_FILE="$HOME/.s3cfg"

echo "Configuring s3cmd for MinIO..."
echo ""

# Check if s3cmd is installed
if ! command -v s3cmd &> /dev/null; then
    echo "s3cmd is not installed. Installing..."

    # Detect OS
    OS="$(uname -s)"
    case "${OS}" in
        Linux*)
            if command -v apt-get &> /dev/null; then
                sudo apt-get update
                sudo apt-get install -y s3cmd
            elif command -v yum &> /dev/null; then
                sudo yum install -y s3cmd
            else
                pip3 install s3cmd
            fi
            ;;
        Darwin*)
            brew install s3cmd || pip3 install s3cmd
            ;;
        *)
            echo "Unsupported OS: ${OS}"
            exit 1
            ;;
    esac

    echo "s3cmd installed successfully!"
else
    echo "s3cmd is already installed"
fi

echo ""

# Backup existing s3cfg if it exists
if [ -f "$S3CFG_FILE" ]; then
    BACKUP_FILE="$S3CFG_FILE.backup.$(date +%Y%m%d_%H%M%S)"
    echo "Backing up existing s3cmd config to: $BACKUP_FILE"
    cp "$S3CFG_FILE" "$BACKUP_FILE"
fi

# Copy template to home directory
echo "Creating s3cmd config at: $S3CFG_FILE"
cp "$S3CFG_TEMPLATE" "$S3CFG_FILE"

echo ""
echo "s3cmd configured successfully for MinIO!"
echo ""
echo "Test the configuration with:"
echo "  s3cmd ls"
echo ""
echo "You should see the ryu-test and ryugraph-test buckets"
echo ""
