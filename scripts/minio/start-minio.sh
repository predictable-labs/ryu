#!/bin/bash
# Start MinIO server using Docker Compose

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Starting MinIO server..."
echo ""

# Check if Docker is running
if ! docker info > /dev/null 2>&1; then
    echo "Error: Docker is not running. Please start Docker first."
    exit 1
fi

# Start MinIO using docker-compose
cd "$SCRIPT_DIR"
docker-compose up -d

echo ""
echo "Waiting for MinIO to be ready..."
sleep 5

# Wait for MinIO to be healthy
COUNTER=0
MAX_RETRIES=30
until curl -s http://localhost:9000/minio/health/live > /dev/null 2>&1; do
    COUNTER=$((COUNTER+1))
    if [ $COUNTER -ge $MAX_RETRIES ]; then
        echo "Error: MinIO failed to start within expected time"
        docker-compose logs minio
        exit 1
    fi
    echo "Waiting for MinIO... ($COUNTER/$MAX_RETRIES)"
    sleep 1
done

echo ""
echo "MinIO started successfully!"
echo ""
echo "MinIO API:     http://localhost:9000"
echo "MinIO Console: http://localhost:9001"
echo "Username:      minioadmin"
echo "Password:      minioadmin"
echo ""
echo "View logs with: docker-compose -f $SCRIPT_DIR/docker-compose.yml logs -f"
echo "Stop with:      docker-compose -f $SCRIPT_DIR/docker-compose.yml down"
echo ""
