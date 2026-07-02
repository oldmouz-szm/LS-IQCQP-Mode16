#!/usr/bin/env bash
set -u

MODE=${1:?usage: benchmark_large_concurrent_mode_vs_original.sh MODE CUTOFF RESULT_NAME INSTANCE_FILE}
CUTOFF=${2:?usage: benchmark_large_concurrent_mode_vs_original.sh MODE CUTOFF RESULT_NAME INSTANCE_FILE}
RESULT_NAME=${3:?usage: benchmark_large_concurrent_mode_vs_original.sh MODE CUTOFF RESULT_NAME INSTANCE_FILE}
SOURCE_INSTANCE_FILE=${4:?usage: benchmark_large_concurrent_mode_vs_original.sh MODE CUTOFF RESULT_NAME INSTANCE_FILE}
PARALLEL_PAIRS=6
TABU=1
BLOCK_SIZE=3
SEED_ARG=8

ROOT="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP"
DATA_DIR="$ROOT/data/all_lp"
NEW_EXEC="$ROOT/src/LS-IQCQP/build/LS-IQCQP"
OLD_EXEC="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
RESULTS_DIR="$ROOT/results/$RESULT_NAME"
INSTANCE_FILE="$RESULTS_DIR/selected_instances.txt"
STATUS_FILE="$RESULTS_DIR/status.txt"

mkdir -p "$RESULTS_DIR/logs"
cp "$SOURCE_INSTANCE_FILE" "$INSTANCE_FILE"
{
    echo "state=running"
    echo "started_at=$(date --iso-8601=seconds)"
    echo "mode=$MODE"
    echo "cutoff_seconds=$CUTOFF"
    echo "parallel_pairs=$PARALLEL_PAIRS"
    echo "pair_execution=concurrent"
} > "$STATUS_FILE"

run_one() {
    local label=$1 inst=$2
    shift 2
    local log="$RESULTS_DIR/logs/${inst}.${label}.log"
    local start end wall exit_code
    start=$(date +%s.%N)
    timeout --signal=TERM --kill-after=10s "$((CUTOFF + 60))s" "$@" > "$log" 2>&1
    exit_code=$?
    end=$(date +%s.%N)
    wall=$(awk -v start="$start" -v end="$end" 'BEGIN {printf "%.6f", end-start}')
    {
        echo
        echo "benchmark_exit_code = $exit_code"
        echo "wall_time_seconds = $wall"
    } >> "$log"
}

run_pair() {
    local inst=$1 file="$DATA_DIR/$1"
    run_one new "$inst" "$NEW_EXEC" "$CUTOFF" "$TABU" "$file" 1 "$MODE" "$BLOCK_SIZE" "$SEED_ARG" &
    local new_pid=$!
    run_one old "$inst" "$OLD_EXEC" "$CUTOFF" "$TABU" "$file" &
    local old_pid=$!
    wait "$new_pid"
    wait "$old_pid"
    echo "completed=$inst" >> "$STATUS_FILE"
}

export MODE CUTOFF TABU BLOCK_SIZE SEED_ARG DATA_DIR NEW_EXEC OLD_EXEC RESULTS_DIR STATUS_FILE
export -f run_one run_pair
xargs -d '\n' -P "$PARALLEL_PAIRS" -I '{}' bash -c 'run_pair "$1"' _ '{}' < "$INSTANCE_FILE"

python3 "$ROOT/scripts/parse_mode13_vs_original_50.py" "$RESULTS_DIR"
{
    echo "finished_at=$(date --iso-8601=seconds)"
    echo "state=done"
} >> "$STATUS_FILE"

