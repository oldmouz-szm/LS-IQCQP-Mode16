#!/bin/bash
set -e

CUTOFF=${1:-10}
DATA_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp"
RESULTS_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_all_serial"
EXEC="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
OLD_EXEC="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"

mkdir -p "$RESULTS_DIR"
echo "Starting tests on all instances..." > "$RESULTS_DIR/status.txt"

# Get all lp files
mapfile -t instances < <(ls -1 "$DATA_DIR" | grep "\.lp$")

for inst in "${instances[@]}"; do
    file="$DATA_DIR/$inst"
    
    # Run Mode 12 (Adaptive Feasible-lock)
    start=$(date +%s.%N)
    $EXEC "$CUTOFF" 1 "$file" 1 12 3 8 > "$RESULTS_DIR/${inst}_mode12.log" 2>&1
    end=$(date +%s.%N)
    wall=$(echo "$end - $start" | bc -l)
    echo "wall_time_seconds = $wall" >> "$RESULTS_DIR/${inst}_mode12.log"
    
    # Run Baseline (Legacy codebase)
    start=$(date +%s.%N)
    $OLD_EXEC "$CUTOFF" 1 "$file" > "$RESULTS_DIR/${inst}_baseline.log" 2>&1
    end=$(date +%s.%N)
    wall=$(echo "$end - $start" | bc -l)
    echo "wall_time_seconds = $wall" >> "$RESULTS_DIR/${inst}_baseline.log"

done

echo "Running python parser..."
python3 scripts/parse_all_serial.py "$RESULTS_DIR"
echo "Done" >> "$RESULTS_DIR/status.txt"
