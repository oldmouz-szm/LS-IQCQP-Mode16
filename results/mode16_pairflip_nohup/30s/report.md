# Mode 16 Pair Flip 30s Report

- Instances: 221
- all: Mode16 vs baseline 26/191/4, Mode16 vs Mode15 24/191/6
- qubo_cache_active: Mode16 vs baseline 24/16/2, Mode16 vs Mode15 21/17/4
- chimera: Mode16 vs baseline 15/2/2, Mode16 vs Mode15 13/3/3
- chimera_qubo_cache_active: Mode16 vs baseline 15/2/2, Mode16 vs Mode15 13/3/3

## Pair Flip And Cache
- Pair attempts/checked/executed: 399484/600197635/90019
- Pair improved-best count: 1077
- Pair time spent sum: 310.206s
- Mode16 cache mismatch sum: 0
- Mode15 cache mismatch sum: 0

## Largest Mode16 vs Mode15 Losses
- QPLIB_2880.lp: mode15=1286880.0, mode16=1317494.0, sense=min, pair_exec=0
- chimera_mgw-c16-2031-01.lp: mode15=1980.0, mode16=1975.0, sense=max, pair_exec=220
- chimera_mgw-c16-2031-02.lp: mode15=1994.0, mode16=1992.0, sense=max, pair_exec=254
- chimera_mgw-c8-507-onc8-02.lp: mode15=495.0, mode16=493.0, sense=max, pair_exec=1448
- QPLIB_3693.lp: mode15=1150.0, mode16=1148.0, sense=max, pair_exec=798
- st_testgr1.lp: mode15=-12.4102, mode16=-12.38955, sense=min, pair_exec=0

## Files
- summary.csv
- aggregate.csv
