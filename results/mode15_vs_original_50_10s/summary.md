# Mode 13 vs Original LS-IQCQP (50 random instances, 10s)

- Mode 13 feasible: 48/50
- Original feasible: 48/50
- Mode 13 wins / ties / losses: 2 / 44 / 2
- Mode 13 only feasible: 0
- Original only feasible: 0
- Neither feasible: 2
- Nonzero exits (Mode 13 / original): 0 / 0

## Aggregated Mode 13 repair statistics

- repair_considered_constraints_total: 9423
- repair_selected_constraints_total: 9423
- repair_skipped_by_topk_total: 0
- safe_accept_checked_total: 138073
- safe_harmful_prevented_total: 20421
- safe_rejected_new_sat_violation_total: 0
- safe_rejected_no_violation_gain_total: 39161
- safe_rejected_total: 39161

## Per-instance results

| Instance | Sense | Mode 13 | Original | Result | Exit (new/old) | Wall s (new/old) |
|---|---:|---:|---:|---|---:|---:|
| crossdock_15x8.lp | min | 16351 | 16351 | tie | 0/0 | 10.025/10.019 |
| st_test1.lp | min | 0 | 0 | tie | 0/0 | 10.012/10.010 |
| QPLIB_3838.lp | max | 740 | 740 | tie | 0/0 | 10.202/10.020 |
| chimera_lga-02.lp | max | 122.7 | 123.9 | new loss | 0/0 | 10.620/10.635 |
| st_test4.lp | min | -7 | -7 | tie | 0/0 | 10.010/10.012 |
| nvs23.lp | min | -1125.2 | -1125.2 | tie | 0/0 | 10.014/10.009 |
| graphpart_2g-0055-0062.lp | min | -1116960 | -1116960 | tie | 0/0 | 10.010/10.013 |
| QPLIB_5980.lp | max | 589 | 589 | tie | 0/0 | 11.781/12.061 |
| chimera_selby-c8-onc8-02.lp | max | 166 | 166 | tie | 0/0 | 10.065/10.076 |
| QPLIB_3850.lp | max | 1188 | 1186 | new win | 0/0 | 10.078/10.053 |
| QPLIB_3738.lp | max | 420 | 420 | tie | 0/0 | 10.011/10.017 |
| QPLIB_3883.lp | min | -765 | -765 | tie | 0/0 | 10.140/10.096 |
| QPLIB_2957.lp | min | 3666 | 3666 | tie | 0/0 | 11.533/11.907 |
| QPLIB_6487.lp | min | 345016 | 345016 | tie | 0/0 | 10.368/12.534 |
| QPLIB_10042.lp | min | -20.881926 | -20.881926 | tie | 0/0 | 10.529/10.568 |
| chimera_mgw-c8-507-onc8-02.lp | max | 494 | 494 | tie | 0/0 | 10.057/10.325 |
| QPLIB_10063.lp | min | -42.515972 | -42.515972 | tie | 0/0 | 10.739/10.475 |
| graphpart_2pm-0055-0055.lp | min | -20 | -20 | tie | 0/0 | 10.008/10.012 |
| QPLIB_2073.lp | min | 7745280 | 7745280 | tie | 0/0 | 10.139/10.523 |
| QPLIB_7154.lp | min | 1159 | 1159 | tie | 0/0 | 10.148/10.143 |
| chimera_k64ising-01.lp | max | 12.9 | 12 | new win | 0/0 | 10.195/10.084 |
| QPLIB_10041.lp | min | 0 | 0 | tie | 0/0 | 10.278/10.277 |
| st_test8.lp | min | -29605 | -29605 | tie | 0/0 | 10.017/10.014 |
| pb351535.lp | min | 5115148 | 5115148 | tie | 0/0 | 10.844/11.328 |
| graphpart_2pm-0099-0999.lp | min | -62 | -62 | tie | 0/0 | 10.021/10.028 |
| chimera_selby-c16-01.lp | max | 708.6 | 710.1 | new loss | 0/0 | 10.503/10.284 |
| graphpart_2g-1010-0824.lp | min | -6960353 | -6960353 | tie | 0/0 | 10.033/10.046 |
| QPLIB_10070.lp | min | -25.258817 | -25.258817 | tie | 0/0 | 10.707/11.041 |
| QPLIB_3347.lp | min | 4015793 | 4015793 | tie | 0/0 | 12.209/12.269 |
| ball_mk3_30.lp | N/A | N/A | N/A | neither feasible | 0/0 | 10.011/10.009 |
| graphpart_2pm-0077-0777.lp | min | -38 | -38 | tie | 0/0 | 10.014/10.018 |
| nvs15.lp | min | 1 | 1 | tie | 0/0 | 10.008/10.007 |
| QPLIB_3923.lp | min | 91.325 | 91.325 | tie | 0/0 | 10.034/10.037 |
| QPLIB_3706.lp | max | 680 | 680 | tie | 0/0 | 10.108/10.031 |
| QPLIB_1976.lp | min | -9594 | -9594 | tie | 0/0 | 10.056/10.043 |
| QPLIB_3822.lp | max | 846 | 846 | tie | 0/0 | 10.224/10.107 |
| graphpart_3g-0244-0244.lp | min | -2540313 | -2540313 | tie | 0/0 | 10.011/10.017 |
| chimera_mgw-c8-439-onc8-001.lp | max | 403 | 403 | tie | 0/0 | 10.218/10.131 |
| graphpart_3pm-0244-0244.lp | min | -28 | -28 | tie | 0/0 | 10.020/10.021 |
| graphpart_2g-0066-0066.lp | min | -2560722 | -2560722 | tie | 0/0 | 10.013/10.009 |
| QPLIB_3565.lp | max | 282 | 282 | tie | 0/0 | 10.049/10.049 |
| QPLIB_10055.lp | min | -1.258548 | -1.258548 | tie | 0/0 | 10.633/10.248 |
| QPLIB_9030.lp | min | 526240 | 526240 | tie | 0/0 | 14.694/14.570 |
| QPLIB_10068.lp | min | -31.624223 | -31.624223 | tie | 0/0 | 10.831/11.517 |
| ball_mk4_15.lp | N/A | N/A | N/A | neither feasible | 0/0 | 10.033/10.076 |
| graphpart_2pm-0088-0888.lp | min | -54 | -54 | tie | 0/0 | 10.019/10.012 |
| QPLIB_10050.lp | min | -25.697662 | -25.697662 | tie | 0/0 | 10.349/10.169 |
| QPLIB_10064.lp | min | 0 | 0 | tie | 0/0 | 10.787/10.699 |
| QPLIB_3757.lp | min | -511 | -511 | tie | 0/0 | 10.080/10.140 |
| pb351555.lp | min | 4888133 | 4888133 | tie | 0/0 | 12.504/13.069 |
