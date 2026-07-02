#!/bin/bash
set -e

CUTOFF=${1:-10}
DATA_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp"
RESULTS_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/block_adaptive_tuning"
EXEC="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
OLD_EXEC="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"

mkdir -p "$RESULTS_DIR"

if [ ! -f "scripts/focus_instances.txt" ]; then
    echo "Error: scripts/focus_instances.txt not found."
    exit 1
fi

cat << 'EOF' > "$RESULTS_DIR/run_job.sh"
#!/bin/bash
CUTOFF=$1
EXEC=$2
FILE=$3
MODE=$4
SEED=$5
OUT_LOG=$6

start=$(date +%s.%N)
if [ "$MODE" = "baseline" ]; then
    $EXEC "$CUTOFF" 1 "$FILE" > "$OUT_LOG" 2>&1
else
    $EXEC "$CUTOFF" 1 "$FILE" 1 "$MODE" 3 "$SEED" > "$OUT_LOG" 2>&1
fi
end=$(date +%s.%N)
wall=$(echo "$end - $start" | bc -l)
echo "wall_time_seconds = $wall" >> "$OUT_LOG"
EOF
chmod +x "$RESULTS_DIR/run_job.sh"

mapfile -t instances < scripts/focus_instances.txt
seeds=(8 9 10)
modes=(6 10 11 12)

echo "Generating jobs..."
rm -f "$RESULTS_DIR/jobs.txt"

for inst in "${instances[@]}"; do
    file="$DATA_DIR/$inst"
    
    # baseline
    echo "\"$RESULTS_DIR/run_job.sh\" \"$CUTOFF\" \"$OLD_EXEC\" \"$file\" \"baseline\" \"8\" \"$RESULTS_DIR/${inst}.cutoff${CUTOFF}.mode_baseline.seed8.log\"" >> "$RESULTS_DIR/jobs.txt"

    for mode in "${modes[@]}"; do
        for seed in "${seeds[@]}"; do
            echo "\"$RESULTS_DIR/run_job.sh\" \"$CUTOFF\" \"$EXEC\" \"$file\" \"$mode\" \"$seed\" \"$RESULTS_DIR/${inst}.cutoff${CUTOFF}.mode${mode}.seed${seed}.log\"" >> "$RESULTS_DIR/jobs.txt"
        done
    done
done

echo "Running tests in parallel..."
cat "$RESULTS_DIR/jobs.txt" | xargs -L 1 -I {} -P 8 bash -c "{}"

echo "Running python parser..."
python3 scripts/parse_focus_results.py "$RESULTS_DIR"
echo "Done. Results saved to $RESULTS_DIR"
