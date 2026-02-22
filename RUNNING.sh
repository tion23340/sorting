#!/bin/bash

FILTER="${1:-BM_Sort_Advanced}"
IMAGE="cpp_benchmark"

if ! docker image inspect $IMAGE &> /dev/null; then
    echo "==> Building Docker image..."
    docker build -t $IMAGE .
fi

echo "==> Running benchmarks with filter: $FILTER"
docker run --rm $IMAGE \
    --benchmark_filter="$FILTER" \
    --benchmark_counters_tabular=true