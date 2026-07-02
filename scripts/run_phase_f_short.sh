#!/bin/bash

CUTOFFS=(10)
SEEDS=(8)
TABU=1
BLOCK_SIZE=3

DATA_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp"
RESULTS_DIR="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/block_adaptive"
EXECUTABLE="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
INSTANCE_FILE="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/scripts/phase_f_instances_short.txt"

mkdir -p "$RESULTS_DIR"
mapfile -t instances < "$INSTANCE_FILE"

configs=(
    "baseline 0 0 0"
    "full_block 1 0 3"
    "mode_6_adaptive_repair 1 6 3"
    "mode_7_adaptive_both 1 7 3"
    "mode_8_adaptive_orig_score 1 8 3"
    "mode_9_legacy_safe 1 9 3"
)

commands_file=$(mktemp)

for cutoff in "${CUTOFFS[@]}"; do
    for inst in "${instances[@]}"; do
        file="$DATA_DIR/$inst"
        for seed in "${SEEDS[@]}"; do
            for config in "${configs[@]}"; do
                read -r name block mode size <<< "$config"
                logfile="$RESULTS_DIR/${inst}.${cutoff}.${name}.seed_${seed}.log"
                
                cmd="(start_time=\$(date +%s.%N); $EXECUTABLE $cutoff $TABU \"$file\" $block $mode $size $seed > \"$logfile\" 2>&1; end_time=\$(date +%s.%N); wall_time=\$(echo \"\$end_time - \$start_time\" | bc -l 2>/dev/null || awk \"BEGIN {print \$end_time - \$start_time}\"); echo \"wall_time_seconds = \$wall_time\" >> \"$logfile\")"
                echo "$cmd" >> "$commands_file"
            done
        done
    done
done

num_cmds=$(wc -l < "$commands_file")
echo "Total tasks to run for Phase F (short): $num_cmds"
if [ "$num_cmds" -gt 0 ]; then
    xargs -P 8 -I {} sh -c "{}" < "$commands_file"
fi

rm -f "$commands_file"
echo "Phase F short experiments completed."
