#!/usr/bin/env bash

set -e

timestamp=$(date +"%Y-%m-%d_%H-%M-%S")
output_dir="benchmarks/results/perf"

mkdir -p "$output_dir"

perf record \
  -F 99 \
  --call-graph dwarf,4096 \
  --debuginfod=system \
  -o "$output_dir/perf_${timestamp}.data" \
  ./out/build/release/benchmarks/http_server_benchmarks 10000 20
