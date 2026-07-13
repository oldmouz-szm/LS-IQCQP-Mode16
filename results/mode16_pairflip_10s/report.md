# Mode 16 Pair Flip 10s Report

- Instances: 221
- all: Mode16 vs baseline 31/185/5, Mode16 vs Mode15 25/190/6
- qubo_cache_active: Mode16 vs baseline 29/12/1, Mode16 vs Mode15 23/16/3
- chimera: Mode16 vs baseline 17/1/1, Mode16 vs Mode15 14/3/2
- chimera_qubo_cache_active: Mode16 vs baseline 17/1/1, Mode16 vs Mode15 14/3/2

## Pair Flip And Cache
- Pair attempts/checked/executed: 163203/246185451/37015
- Pair improved-best count: 1039
- Pair time spent sum: 103.858s
- Mode16 cache mismatch sum: 0
- Mode15 cache mismatch sum: 0

## Largest Mode16 vs Mode15 Losses
- QPLIB_3347.lp: mode15=4015793.0, mode16=4017827.0, sense=min, pair_exec=0
- QPLIB_5944.lp: mode15=1326.0, mode16=1134.0, sense=max, pair_exec=0
- QPLIB_5971.lp: mode15=1045.0, mode16=1036.0, sense=max, pair_exec=0
- chimera_mgw-c16-2031-01.lp: mode15=1969.0, mode16=1966.0, sense=max, pair_exec=114
- chimera_mgw-c8-507-onc8-02.lp: mode15=494.0, mode16=492.0, sense=max, pair_exec=568
- QPLIB_3506.lp: mode15=478.0, mode16=476.0, sense=max, pair_exec=754

## Files
- summary.csv: per-instance objective/status/statistics
- aggregate.csv: aggregate win/tie/loss and pair/cache totals
