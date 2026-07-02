#!/bin/bash
NEW_EXEC="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
OLD_EXEC="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
DATA_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp"
RESULTS_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_test"
mkdir -p "$RESULTS_DIR"

instances=(
  "QPLIB_3347.lp"
  "QPLIB_2880.lp"
  "QPLIB_6597.lp"
  "pb302035.lp"
  "pb302055.lp"
  "pb302075.lp"
)

echo "Starting tests..." > "$RESULTS_DIR/status.txt"

for inst in "${instances[@]}"; do
  file="$DATA_DIR/$inst"
  
  # Run New (Block Move Optimal: full_block mode 0, block_size 2, seed 8)
  echo "Running NEW on $inst..." >> "$RESULTS_DIR/status.txt"
  start=$(date +%s.%N)
  $NEW_EXEC 10 1 "$file" 1 0 2 8 > "$RESULTS_DIR/${inst}_new.log" 2>&1
  end=$(date +%s.%N)
  wall=$(echo "$end - $start" | bc -l)
  echo "wall_time_seconds = $wall" >> "$RESULTS_DIR/${inst}_new.log"
  
  # Run Old
  echo "Running OLD on $inst..." >> "$RESULTS_DIR/status.txt"
  start=$(date +%s.%N)
  $OLD_EXEC 10 1 "$file" > "$RESULTS_DIR/${inst}_old.log" 2>&1
  end=$(date +%s.%N)
  wall=$(echo "$end - $start" | bc -l)
  echo "wall_time_seconds = $wall" >> "$RESULTS_DIR/${inst}_old.log"
done

echo "Done" >> "$RESULTS_DIR/status.txt"

# Simple parser to compare results
cat << 'EOF' > "$RESULTS_DIR/parse_results.py"
import os

def get_best_obj(log_file):
    best_obj = "N/A"
    time = "N/A"
    try:
        with open(log_file, "r") as f:
            for line in f:
                if "best obj min=" in line or "best obj max=" in line:
                    best_obj = line.split("=")[-1].strip()
                elif "wall_time_seconds =" in line:
                    time = line.split("=")[-1].strip()[:6]
    except:
        pass
    return best_obj, time

print("| Instance | Old Project (szm/LS-IQCQP) | Time | New Project (IQCQP-block) | Time |")
print("|---|---|---|---|---|")
instances = ["QPLIB_3347.lp", "QPLIB_2880.lp", "QPLIB_6597.lp", "pb302035.lp", "pb302055.lp", "pb302075.lp"]
results_dir = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_test"

for inst in instances:
    old_log = os.path.join(results_dir, f"{inst}_old.log")
    new_log = os.path.join(results_dir, f"{inst}_new.log")
    
    old_obj, old_time = get_best_obj(old_log)
    new_obj, new_time = get_best_obj(new_log)
    
    print(f"| {inst} | {old_obj} | {old_time}s | {new_obj} | {new_time}s |")
EOF

python3 "$RESULTS_DIR/parse_results.py" > "$RESULTS_DIR/summary.md"
