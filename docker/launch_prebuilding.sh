#! /usr/bin/env bash
set -euo pipefail

# Parse input params
python_version=${1:-lastest}
cuda_version=${2:-11.8}
gpu_backend=${3:-cuda}

export FF_CUDA_ARCH=all
export BUILD_LEGION_ONLY=ON

# Build Docker Flexflow Container
echo "building docker"
./docker/build.sh flexflow
export detached=ON
./docker/run.sh flexflow

# Copy legion libraries to host
echo "copy legion libaries"
echo "flexflow-${gpu_backend}-${cuda_version}:${python_version}"
container_id=$(docker ps | grep flexflow-cuda-11.8:latest | awk '{print $1}')
echo "$container_id"
docker cp "$container_id":/usr/FlexFlow/build/deps ~/buildlegion

# Create the tarball file
cd ~/buildlegion
export LEGION_TARBALL="legion_ubuntu-20.04_${gpu_backend}.tar.gz"
echo "Creating archive $LEGION_TARBALL"
touch "$LEGION_TARBALL"
tar --exclude="$LEGION_TARBALL" -zcvf "$LEGION_TARBALL" .
echo "Checking the size of the Legion tarball..."
du -h "$LEGION_TARBALL"

# Stop and remove the container
docker stop $container_id
docker rm $container_id