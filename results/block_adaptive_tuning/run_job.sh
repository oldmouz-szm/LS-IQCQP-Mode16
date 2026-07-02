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
