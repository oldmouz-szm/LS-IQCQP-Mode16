# Mode 15 vs Original LS-IQCQP (50 instances, 10s)

- Mode 15 feasible: 50/50
- Original feasible: 50/50
- Mode 15 wins / ties / losses: 3 / 46 / 1
- Mode 15 only feasible: 0
- Original only feasible: 0
- Neither feasible: 0
- Nonzero exits (Mode 15 / original): 0 / 0

## Aggregated Mode 15 repair statistics

- qubo_gain_cache_hits_total: 0
- qubo_gain_cache_mismatch_total: 0
- qubo_gain_cache_neighbor_updates_total: 0
- qubo_gain_cache_rebuild_total: 0
- repair_considered_constraints_total: 0
- repair_selected_constraints_total: 0
- repair_skipped_by_topk_total: 0
- safe_accept_checked_total: 0
- safe_harmful_prevented_total: 0
- safe_rejected_new_sat_violation_total: 0
- safe_rejected_no_violation_gain_total: 0
- safe_rejected_total: 0
- serial_diversification_total: 3

## Per-instance results

| Instance | Sense | Mode 15 | Original | Result | Exit (new/old) | Wall s (new/old) |
|---|---:|---:|---:|---|---:|---:|
| QPLIB_3775.lp | min | 3990 | 3990 | tie | 0/0 | 10.168/10.123 |
| prob02.lp | min | 144265 | 144265 | tie | 0/0 | 10.017/10.014 |
| nvs11.lp | min | -431 | -431 | tie | 0/0 | 10.011/10.010 |
| ball_mk2_30.lp | min | 0 | 0 | tie | 0/0 | 10.009/10.012 |
| QPLIB_3803.lp | min | -6982 | -6982 | tie | 0/0 | 10.064/10.146 |
| nvs18.lp | min | -778.4 | -778.4 | tie | 0/0 | 10.018/10.012 |
| QPLIB_10069.lp | min | 0 | 0 | tie | 0/0 | 11.151/11.315 |
| QPLIB_3650.lp | max | 914 | 916 | new loss | 0/0 | 10.113/10.116 |
| QPLIB_5725.lp | max | 33065571 | 33065571 | tie | 0/0 | 10.108/10.073 |
| st_miqp3.lp | min | -6 | -6 | tie | 0/0 | 10.007/10.007 |
| QPLIB_6764.lp | min | 221 | 221 | tie | 0/0 | 12.211/12.412 |
| chimera_lga-01.lp | max | 123.3 | 123.3 | tie | 0/0 | 10.417/10.751 |
| QPLIB_6799.lp | min | 165 | 165 | tie | 0/0 | 11.823/12.109 |
| QPLIB_3705.lp | max | 382 | 382 | tie | 0/0 | 10.051/10.110 |
| QPLIB_3307.lp | min | 1282 | 1282 | tie | 0/0 | 10.362/10.334 |
| QPLIB_3750.lp | min | 6348 | 6348 | tie | 0/0 | 10.431/10.231 |
| nvs13.lp | min | -585.2 | -585.2 | tie | 0/0 | 10.020/10.011 |
| st_testph4.lp | min | -80.5 | -80.5 | tie | 0/0 | 10.010/10.010 |
| QPLIB_3931.lp | min | 113.3365 | 113.3365 | tie | 0/0 | 10.023/10.094 |
| QPLIB_2315.lp | min | -27096 | -27096 | tie | 0/0 | 10.911/11.062 |
| QPLIB_10052.lp | min | -11.810576 | -11.810576 | tie | 0/0 | 10.486/10.581 |
| QPLIB_10060.lp | min | 0 | 0 | tie | 0/0 | 10.993/11.288 |
| chimera_mgw-c8-439-onc8-002.lp | max | 392 | 392 | tie | 0/0 | 10.299/10.097 |
| QPLIB_0067.lp | min | -94446 | -94446 | tie | 0/0 | 10.026/10.024 |
| QPLIB_10072.lp | min | 0 | 0 | tie | 0/0 | 10.091/10.038 |
| QPLIB_6941.lp | min | 214 | 214 | tie | 0/0 | 13.057/13.223 |
| QPLIB_3584.lp | min | -22989 | -22989 | tie | 0/0 | 10.628/10.738 |
| QPLIB_10062.lp | min | 0 | 0 | tie | 0/0 | 10.121/10.167 |
| QPLIB_10067.lp | min | -31.568701 | -31.568701 | tie | 0/0 | 10.804/11.038 |
| QPLIB_7127.lp | min | 0 | 0 | tie | 0/0 | 10.224/10.377 |
| chimera_selby-c8-onc8-01.lp | max | 175 | 174.9 | new win | 0/0 | 10.024/10.050 |
| QPLIB_2029.lp | min | -34576 | -34576 | tie | 0/0 | 10.094/10.130 |
| QPLIB_10074.lp | min | 0 | 0 | tie | 0/0 | 10.195/10.041 |
| QPLIB_10057.lp | min | 0 | 0 | tie | 0/0 | 10.629/10.990 |
| QPLIB_3402.lp | min | 229092 | 229092 | tie | 0/0 | 10.183/10.058 |
| QPLIB_10061.lp | min | -23.212658 | -23.212658 | tie | 0/0 | 10.447/10.706 |
| pb351575.lp | min | 6553485 | 6553485 | tie | 0/0 | 12.739/13.099 |
| QPLIB_2067.lp | min | 3740230 | 3740230 | tie | 0/0 | 10.145/10.162 |
| QPLIB_2880.lp | min | 1286880 | 1286880 | tie | 0/0 | 10.517/10.486 |
| chimera_selby-c16-02.lp | max | 696.2 | 693.9 | new win | 0/0 | 10.603/10.302 |
| QPLIB_2087.lp | min | 3312579 | 3312579 | tie | 0/0 | 10.802/10.620 |
| chimera_mgw-c8-507-onc8-01.lp | max | 493 | 493 | tie | 0/0 | 10.368/10.308 |
| QPLIB_3913.lp | min | 44.6 | 44.6 | tie | 0/0 | 10.682/10.725 |
| nvs03.lp | min | 64 | 64 | tie | 0/0 | 10.008/10.009 |
| QPLIB_2085.lp | min | 7483750 | 7483750 | tie | 0/0 | 10.665/10.135 |
| graphpart_3g-0333-0333.lp | min | -1754342 | -1754342 | tie | 0/0 | 10.014/10.017 |
| QPLIB_5755.lp | max | 23749203 | 23692136 | new win | 0/0 | 10.060/10.026 |
| QPLIB_5962.lp | max | 3017 | 3017 | tie | 0/0 | 11.247/10.848 |
| QPLIB_10058.lp | min | -3.524152 | -3.524152 | tie | 0/0 | 10.112/10.109 |
| QPLIB_10049.lp | min | 0 | 0 | tie | 0/0 | 10.492/10.592 |
