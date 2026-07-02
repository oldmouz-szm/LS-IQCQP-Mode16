#!/usr/bin/env bash
set -u

MODE=${1:?usage: benchmark_current_mode_against_saved_original.sh MODE RESULT_NAME [BASELINE_DIR] [CUTOFF]}
RESULT_NAME=${2:?usage: benchmark_current_mode_against_saved_original.sh MODE RESULT_NAME [BASELINE_DIR] [CUTOFF]}
CUTOFF=${4:-10}
PARALLEL=8
TABU=1
BLOCK_SIZE=3
SEED_ARG=8

ROOT="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP"
DATA_DIR="$ROOT/data/all_lp"
EXEC="$ROOT/src/LS-IQCQP/build/LS-IQCQP"
BASELINE_DIR=${3:-"$ROOT/results/mode13_vs_original_50_10s"}
RESULTS_DIR="$ROOT/results/$RESULT_NAME"
STATUS_FILE="$RESULTS_DIR/status.txt"

mkdir -p "$RESULTS_DIR/logs"
cp "$BASELINE_DIR/selected_instances.txt" "$RESULTS_DIR/selected_instances.txt"
cp "$BASELINE_DIR"/logs/*.old.log "$RESULTS_DIR/logs/"

{
    echo "state=running"
    echo "started_at=$(date --iso-8601=seconds)"
    echo "mode=$MODE"
    echo "cutoff_seconds=$CUTOFF"
} > "$STATUS_FILE"

run_current() {
    local inst=$1
    local file="$DATA_DIR/$inst"
    local log="$RESULTS_DIR/logs/${inst}.new.log"
    local start end wall exit_code

    start=$(date +%s.%N)
    timeout --signal=TERM --kill-after=5s "$((CUTOFF + 30))s" \
        "$EXEC" "$CUTOFF" "$TABU" "$file" 1 "$MODE" "$BLOCK_SIZE" "$SEED_ARG" \
        > "$log" 2>&1
    exit_code=$?
    end=$(date +%s.%N)
    wall=$(awk -v start="$start" -v end="$end" 'BEGIN {printf "%.6f", end-start}')
    {
        echo
        echo "benchmark_exit_code = $exit_code"
        echo "wall_time_seconds = $wall"
    } >> "$log"
    echo "completed=$inst" >> "$STATUS_FILE"
}

export MODE CUTOFF TABU BLOCK_SIZE SEED_ARG DATA_DIR EXEC RESULTS_DIR STATUS_FILE
export -f run_current
xargs -d '\n' -P "$PARALLEL" -I '{}' bash -c 'run_current "$1"' _ '{}' \
    < "$RESULTS_DIR/selected_instances.txt"

python3 "$ROOT/scripts/parse_mode13_vs_original_50.py" "$RESULTS_DIR"
{
    echo "finished_at=$(date --iso-8601=seconds)"
    echo "state=done"
} >> "$STATUS_FILE"
