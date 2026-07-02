#!/usr/bin/env bash
set -u

CUTOFF=10
PARALLEL_PAIRS=8
TABU=1
MODE=13
BLOCK_SIZE=3
SEED=8

ROOT="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP"
DATA_DIR="$ROOT/data/all_lp"
NEW_EXEC="$ROOT/src/LS-IQCQP/build/LS-IQCQP"
OLD_EXEC="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
RESULTS_DIR="$ROOT/results/mode13_vs_original_50_10s"
INSTANCE_FILE="$RESULTS_DIR/selected_instances.txt"
STATUS_FILE="$RESULTS_DIR/status.txt"

mkdir -p "$RESULTS_DIR/logs"

if [[ ! -x "$NEW_EXEC" || ! -x "$OLD_EXEC" ]]; then
    echo "Both solver executables must exist and be executable." >&2
    exit 1
fi

# The sampled list is persisted so both solvers always receive exactly the same inputs.
if [[ ! -s "$INSTANCE_FILE" ]]; then
    find "$DATA_DIR" -maxdepth 1 -type f -name '*.lp' -printf '%f\n' \
        | shuf -n 50 > "$INSTANCE_FILE"
fi

instance_count=$(awk 'NF {count++} END {print count+0}' "$INSTANCE_FILE")
if [[ "$instance_count" -ne 50 ]]; then
    echo "Expected 50 selected instances, found $instance_count." >&2
    exit 1
fi

{
    echo "state=running"
    echo "started_at=$(date --iso-8601=seconds)"
    echo "cutoff_seconds=$CUTOFF"
    echo "parallel_pairs=$PARALLEL_PAIRS"
    echo "new_config=tabu:$TABU,mode:$MODE,block_size:$BLOCK_SIZE,seed:$SEED"
    echo "old_config=tabu:$TABU,default_algorithm"
} > "$STATUS_FILE"

run_one() {
    local label=$1
    local inst=$2
    local log="$RESULTS_DIR/logs/${inst}.${label}.log"
    shift 2

    local start end wall exit_code
    start=$(date +%s.%N)
    timeout --signal=TERM --kill-after=5s 30s "$@" > "$log" 2>&1
    exit_code=$?
    end=$(date +%s.%N)
    wall=$(awk -v start="$start" -v end="$end" 'BEGIN {printf "%.6f", end-start}')
    {
        # Some solver paths do not terminate their final output line.
        echo
        echo "benchmark_exit_code = $exit_code"
        echo "wall_time_seconds = $wall"
    } >> "$log"
}

run_pair() {
    local inst=$1
    local file="$DATA_DIR/$inst"

    run_one new "$inst" \
        "$NEW_EXEC" "$CUTOFF" "$TABU" "$file" 1 "$MODE" "$BLOCK_SIZE" "$SEED"
    run_one old "$inst" \
        "$OLD_EXEC" "$CUTOFF" "$TABU" "$file"

    echo "completed=$inst" >> "$STATUS_FILE"
}

export CUTOFF TABU MODE BLOCK_SIZE SEED DATA_DIR NEW_EXEC OLD_EXEC RESULTS_DIR STATUS_FILE
export -f run_one run_pair

xargs -d '\n' -P "$PARALLEL_PAIRS" -I '{}' bash -c 'run_pair "$1"' _ '{}' \
    < "$INSTANCE_FILE"

python3 "$ROOT/scripts/parse_mode13_vs_original_50.py" "$RESULTS_DIR"

{
    echo "finished_at=$(date --iso-8601=seconds)"
    echo "state=done"
} >> "$STATUS_FILE"
