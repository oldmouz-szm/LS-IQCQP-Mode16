#!/bin/bash
# run_block_ablation.sh <cutoff> <suite> [options or instances_file]
# Options:
#   --resume   Skip existing logs
#   --dry-run  Only print commands
# Env:
#   JOBS=N     Number of parallel jobs (default: 1)

CUTOFF=${1:-10}
SUITE=${2:-smoke}

INSTANCE_FILE=""
RESUME=0
DRY_RUN=0

# Parse remaining arguments
shift 2
for arg in "$@"; do
    if [ "$arg" == "--resume" ]; then
        RESUME=1
    elif [ "$arg" == "--dry-run" ]; then
        DRY_RUN=1
    else
        INSTANCE_FILE="$arg"
    fi
done

DATA_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp"
RESULTS_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/block_ablation"
EXECUTABLE="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
JOBS=${JOBS:-1}

mkdir -p "$RESULTS_DIR"

# Collect instances
instances=()
if [ -n "$INSTANCE_FILE" ] && [ -f "$INSTANCE_FILE" ]; then
    while IFS= read -r line; do
        [[ -n "$line" ]] && instances+=("$line")
    done < "$INSTANCE_FILE"
else
    # Auto scan
    all_files=($(ls $DATA_DIR/*.lp | sort -f))
    
    if [ "$SUITE" == "smoke" ]; then
        MAX_INSTANCES=5
    elif [ "$SUITE" == "medium" ]; then
        MAX_INSTANCES=30
    else
        MAX_INSTANCES=100
    fi
    
    count=0
    for f in "${all_files[@]}"; do
        if [ $count -ge $MAX_INSTANCES ]; then
            break
        fi
        instances+=("$f")
        count=$((count + 1))
    done
fi

echo "Running $SUITE suite on ${#instances[@]} instances. (JOBS=$JOBS)"
if [ "$DRY_RUN" == "1" ]; then
    echo "Dry run enabled. Will not execute commands."
fi

# Configure modes based on suite
if [ "$SUITE" == "smoke" ]; then
    tabu_list=(0)
    seed_list=(8 9)
    block_size_list=(2 3)
    configs=("0 0" "1 0" "1 1" "1 3" "1 5")
else
    # medium or full
    tabu_list=(0 1)
    seed_list=(8 9 10 11 12)
    block_size_list=(2 3 4)
    configs=("0 0" "1 0" "1 1" "1 2" "1 3" "1 4" "1 5")
fi

commands_file=$(mktemp)

for instance in "${instances[@]}"; do
    for tabu in "${tabu_list[@]}"; do
        for config in "${configs[@]}"; do
            read -r block mode <<< "$config"
            if [ "$block" == "0" ]; then
                # Baseline: only run once with block_size=0 (named baseline in mode)
                for seed in "${seed_list[@]}"; do
                    basename=$(basename "$instance")
                    logfile="$RESULTS_DIR/${basename}.${CUTOFF}.${tabu}.${block}.baseline.0.${seed}.log"
                    if [ "$RESUME" == "1" ] && [ -s "$logfile" ] && grep -q "best obj" "$logfile"; then
                        continue
                    fi
                    cmd="(start_time=\$(date +%s.%N); $EXECUTABLE $CUTOFF $tabu \"$instance\" $block $mode 0 $seed > \"$logfile\" 2>&1; end_time=\$(date +%s.%N); wall_time=\$(echo \"\$end_time - \$start_time\" | bc -l 2>/dev/null || awk \"BEGIN {print \$end_time - \$start_time}\"); echo \"wall_time_seconds = \$wall_time\" >> \"$logfile\")"
                    echo "$cmd" >> "$commands_file"
                done
            else
                for size in "${block_size_list[@]}"; do
                    for seed in "${seed_list[@]}"; do
                        basename=$(basename "$instance")
                        logfile="$RESULTS_DIR/${basename}.${CUTOFF}.${tabu}.${block}.${mode}.${size}.${seed}.log"
                        if [ "$RESUME" == "1" ] && [ -s "$logfile" ] && grep -q "block stats:" "$logfile"; then
                            continue
                        fi
                        cmd="(start_time=\$(date +%s.%N); $EXECUTABLE $CUTOFF $tabu \"$instance\" $block $mode $size $seed > \"$logfile\" 2>&1; end_time=\$(date +%s.%N); wall_time=\$(echo \"\$end_time - \$start_time\" | bc -l 2>/dev/null || awk \"BEGIN {print \$end_time - \$start_time}\"); echo \"wall_time_seconds = \$wall_time\" >> \"$logfile\")"
                        echo "$cmd" >> "$commands_file"
                    done
                done
            fi
        done
    done
done

num_cmds=$(wc -l < "$commands_file")
echo "Total tasks to run: $num_cmds"

if [ "$DRY_RUN" == "1" ]; then
    cat "$commands_file" | head -n 10
    echo "..."
elif [ "$num_cmds" -gt 0 ]; then
    xargs -P "$JOBS" -I {} sh -c "{}" < "$commands_file"
fi

rm -f "$commands_file"
echo "Done."
