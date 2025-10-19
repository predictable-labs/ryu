#!/bin/bash
# Stop MinIO server

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Stopping MinIO server..."
echo ""

cd "$SCRIPT_DIR"
docker-compose down

echo ""
echo "MinIO stopped successfully!"
echo ""
echo "To remove all data, run: docker-compose -f $SCRIPT_DIR/docker-compose.yml down -v"
echo ""
