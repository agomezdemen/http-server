#!/usr/bin/env bash

set -e

timestamp=$(date +"%Y-%m-%d_%H-%M-%S")
output_dir="benchmarks/results/wrk"

mkdir -p "$output_dir"

wrk \
  -t4 \
  -c100 \
  -d30s \
  --latency \
  http://127.0.0.1:8080/ \
  | tee "$output_dir/wrk_${timestamp}.txt"
