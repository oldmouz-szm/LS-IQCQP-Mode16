# Mode 16 Pair Flip 300s Report

- Instances: 221
- all: Mode16 vs baseline 18/196/7, Mode16 vs Mode15 17/198/6
- qubo_cache_active: Mode16 vs baseline 18/19/5, Mode16 vs Mode15 15/22/5
- chimera: Mode16 vs baseline 15/3/1, Mode16 vs Mode15 13/4/2
- chimera_qubo_cache_active: Mode16 vs baseline 15/3/1, Mode16 vs Mode15 13/4/2

## Pair Flip And Cache
- Pair attempts/checked/executed: 3967936/5876744549/840283
- Pair improved-best count: 1130
- Pair time spent sum: 3022.514s
- Mode16 cache mismatch sum: 0
- Mode15 cache mismatch sum: 0

## Largest Mode16 vs Mode15 Losses
- QPLIB_5755.lp: mode15=24673183.0, mode16=24555824.0, sense=max, pair_exec=64066
- QPLIB_2029.lp: mode15=-34704.0, mode16=-34576.0, sense=min, pair_exec=0
- chimera_mgw-c16-2031-01.lp: mode15=2002.0, mode16=2000.0, sense=max, pair_exec=2168
- QPLIB_3706.lp: mode15=682.0, mode16=680.0, sense=max, pair_exec=16174
- QPLIB_3822.lp: mode15=850.0, mode16=848.0, sense=max, pair_exec=13109
- chimera_rfr-02.lp: mode15=1163.1, mode16=1162.8, sense=max, pair_exec=11218

## Files
- summary.csv
- aggregate.csv
