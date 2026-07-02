#!/usr/bin/env bash
set -u

MODE=${1:-15}
RESULT_NAME=${2:-full_221_mode15_vs_original}
PARALLEL_PAIRS=${PARALLEL_PAIRS:-12}
TABU=1
BLOCK_SIZE=3
SEED_ARG=8
CUTOFFS=(10 60 300)

ROOT="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP"
DATA_DIR="$ROOT/data/all_lp"
NEW_EXEC="$ROOT/src/LS-IQCQP/build/LS-IQCQP"
OLD_EXEC="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
RESULTS_DIR="$ROOT/results/$RESULT_NAME"
INSTANCE_FILE="$RESULTS_DIR/all_221_instances.txt"
JOBS_FILE="$RESULTS_DIR/jobs.txt"
STATUS_FILE="$RESULTS_DIR/status.txt"

mkdir -p "$RESULTS_DIR"
find "$DATA_DIR" -maxdepth 1 -type f -name '*.lp' -printf '%f\n' | sort > "$INSTANCE_FILE"
instance_count=$(wc -l < "$INSTANCE_FILE")
if [[ "$instance_count" -ne 221 ]]; then
    echo "Expected 221 instances, found $instance_count." >&2
    exit 1
fi

: > "$JOBS_FILE"
for cutoff in "${CUTOFFS[@]}"; do
    cutoff_dir="$RESULTS_DIR/${cutoff}s"
    mkdir -p "$cutoff_dir/logs"
    cp "$INSTANCE_FILE" "$cutoff_dir/selected_instances.txt"
    {
        echo "state=running"
        echo "mode=$MODE"
        echo "cutoff_seconds=$cutoff"
    } > "$cutoff_dir/status.txt"
    while IFS= read -r inst; do
        printf '%s %s\n' "$cutoff" "$inst" >> "$JOBS_FILE"
    done < "$INSTANCE_FILE"
done

{
    echo "state=running"
    echo "started_at=$(date --iso-8601=seconds)"
    echo "mode=$MODE"
    echo "instances=$instance_count"
    echo "cutoffs=10,60,300"
    echo "parallel_pairs=$PARALLEL_PAIRS"
    echo "solver_threads=1"
    echo "pair_execution=concurrent"
    echo "total_runs=$((instance_count * ${#CUTOFFS[@]} * 2))"
} > "$STATUS_FILE"

run_one() {
    local cutoff=$1 label=$2 inst=$3
    shift 3
    local log="$RESULTS_DIR/${cutoff}s/logs/${inst}.${label}.log"

    if [[ -s "$log" ]] && grep -q '^benchmark_exit_code = ' "$log"; then
        return 0
    fi

    local start end wall exit_code
    start=$(date +%s.%N)
    timeout --signal=TERM --kill-after=15s "$((cutoff + 120))s" "$@" > "$log" 2>&1
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
    local cutoff=$1 inst=$2 file="$DATA_DIR/$2"
    run_one "$cutoff" new "$inst" \
        "$NEW_EXEC" "$cutoff" "$TABU" "$file" 1 "$MODE" "$BLOCK_SIZE" "$SEED_ARG" &
    local new_pid=$!
    run_one "$cutoff" old "$inst" \
        "$OLD_EXEC" "$cutoff" "$TABU" "$file" &
    local old_pid=$!
    wait "$new_pid"
    wait "$old_pid"
    echo "completed=$cutoff,$inst" >> "$STATUS_FILE"
    echo "completed=$inst" >> "$RESULTS_DIR/${cutoff}s/status.txt"
}

export MODE TABU BLOCK_SIZE SEED_ARG DATA_DIR NEW_EXEC OLD_EXEC RESULTS_DIR STATUS_FILE
export -f run_one run_pair
xargs -n 2 -P "$PARALLEL_PAIRS" bash -c 'run_pair "$1" "$2"' _ < "$JOBS_FILE"

for cutoff in "${CUTOFFS[@]}"; do
    python3 "$ROOT/scripts/parse_mode13_vs_original_50.py" "$RESULTS_DIR/${cutoff}s"
    {
        echo "finished_at=$(date --iso-8601=seconds)"
        echo "state=done"
    } >> "$RESULTS_DIR/${cutoff}s/status.txt"
done
python3 "$ROOT/scripts/aggregate_full_benchmark.py" "$RESULTS_DIR"

{
    echo "finished_at=$(date --iso-8601=seconds)"
    echo "state=done"
} >> "$STATUS_FILE"

