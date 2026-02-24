#!/bin/bash

FILTER="${1:-BM_Sort_Advanced}"
THREADS="${2:-$(nproc)}"
IMAGE="cpp_benchmark"

if ! docker image inspect $IMAGE &> /dev/null; then
    echo "==> Building Docker image..."
    docker build -t $IMAGE .
fi

echo "==> Running benchmarks with filter: $FILTER"
docker run --rm \
    -e OMP_NUM_THREADS="$THREADS" \
    -e OMP_PROC_BIND=true \
    -e OMP_PLACES=cores \
    $IMAGE \
    --benchmark_filter="$FILTER" \
    --benchmark_counters_tabular=true
