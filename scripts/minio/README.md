# MinIO Local S3 Testing Setup

This directory contains scripts and configuration for running a local MinIO server to test S3 functionality without requiring AWS credentials.

## What is MinIO?

MinIO is a high-performance, S3-compatible object storage server. It's perfect for local development and testing of S3 workflows.

## Quick Start

### 1. Start MinIO

```bash
./scripts/minio/start-minio.sh
```

This will:
- Start a MinIO server via Docker Compose
- Create the required buckets (`ryu-test` and `ryugraph-test`)
- Make the MinIO API available at http://localhost:9000
- Make the MinIO Console (web UI) available at http://localhost:9001

### 2. Configure s3cmd

```bash
./scripts/minio/setup-s3cmd.sh
```

This will:
- Install s3cmd if not already installed
- Configure s3cmd to work with the local MinIO server
- Backup any existing s3cmd configuration

### 3. Test Binary Dataset Generation

```bash
./test-binary-dataset-local.sh
```

This will:
- Generate binary datasets (binary-demo and tinysnb)
- Upload them to MinIO
- Verify the upload/download functionality

## Manual Usage

### List Buckets

```bash
s3cmd ls
```

### List Objects in a Bucket

```bash
s3cmd ls s3://ryu-test/
s3cmd ls s3://ryugraph-test/
```

### Upload a File

```bash
s3cmd put myfile.txt s3://ryu-test/
```

### Download a File

```bash
s3cmd get s3://ryu-test/myfile.txt
```

### Sync a Directory

```bash
s3cmd sync ./my-directory s3://ryu-test/
```

## Using MinIO Client (mc)

You can also use the MinIO client directly:

```bash
# Configure alias
mc alias set local http://localhost:9000 minioadmin minioadmin

# List buckets
mc ls local/

# List objects
mc ls local/ryu-test/

# Upload
mc cp myfile.txt local/ryu-test/

# Download
mc cp local/ryu-test/myfile.txt ./
```

## Web Console

Access the MinIO web console at: http://localhost:9001

- **Username:** minioadmin
- **Password:** minioadmin

From the console, you can:
- Browse buckets and objects
- Upload/download files
- Manage access policies
- Monitor storage usage

## Stopping MinIO

```bash
./scripts/minio/stop-minio.sh
```

### Remove All Data

To stop MinIO and remove all stored data:

```bash
cd scripts/minio
docker-compose down -v
```

## Configuration Files

- `docker-compose.yml` - Docker Compose configuration for MinIO
- `.s3cfg.template` - Template configuration for s3cmd
- `start-minio.sh` - Start MinIO server
- `stop-minio.sh` - Stop MinIO server
- `setup-s3cmd.sh` - Configure s3cmd for MinIO
- `init-minio.sh` - Initialize MinIO (create buckets, etc.)

## Default Credentials

- **Access Key:** minioadmin
- **Secret Key:** minioadmin

## Troubleshooting

### MinIO won't start

Check if the ports are already in use:
```bash
lsof -i :9000
lsof -i :9001
```

### s3cmd connection errors

Verify MinIO is running:
```bash
curl http://localhost:9000/minio/health/live
```

Check your s3cmd configuration:
```bash
cat ~/.s3cfg
```

### Docker errors

Make sure Docker is running:
```bash
docker info
```

View MinIO logs:
```bash
cd scripts/minio
docker-compose logs -f
```

## Environment Variables

When running `test-binary-dataset-local.sh`, you can control MinIO usage:

```bash
# Use MinIO (default)
./test-binary-dataset-local.sh

# Skip MinIO
USE_MINIO=false ./test-binary-dataset-local.sh
```

## Integration with CI

The CI workflow uses real S3 buckets with GitHub Secrets for credentials. This MinIO setup is only for local development and testing.

To mimic the CI environment locally:
1. Start MinIO
2. Configure s3cmd
3. Run the test script

The workflow will be identical to what happens in GitHub Actions, except it uses MinIO instead of AWS S3.
