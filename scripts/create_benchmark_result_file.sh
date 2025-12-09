#!/usr/bin/env bash

set -euo pipefail

if [ $# -ne 1 ]; then
    echo "Usage: $0 <TestName>"
    echo "Example: $0 MemoryPool"
    exit 1
fi

# TODO - Create global env variables and use those instead of relative paths
TEST_NAME="$1"
BENCH_BIN="../build/bin/${TEST_NAME}.bench"
OUT_DIR="../benchmarks"

if [ ! -x "$BENCH_BIN" ]; then
    echo "❌ Benchmark binary not found or not executable:"
    echo "   $BENCH_BIN"
    exit 1
fi

mkdir -p "$OUT_DIR"

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
OUT_FILE="${OUT_DIR}/${TEST_NAME}_bench_${TIMESTAMP}.json"

echo "▶ Running benchmark: $BENCH_BIN"
echo "▶ Output file: $OUT_FILE"

"$BENCH_BIN" \
    --benchmark_out="$OUT_FILE" \
    --benchmark_out_format=json

echo "✅ Benchmark completed successfully"
echo "   Results saved to: $OUT_FILE"