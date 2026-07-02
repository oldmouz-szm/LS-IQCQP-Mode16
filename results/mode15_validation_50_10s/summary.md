# Mode 15 vs Original LS-IQCQP (50 random instances, 10s)

- Mode 15 feasible: 50/50
- Original feasible: 50/50
- Mode 15 wins / ties / losses: 3 / 44 / 3
- Mode 15 only feasible: 0
- Original only feasible: 0
- Neither feasible: 0
- Nonzero exits (Mode 15 / original): 0 / 0

## Aggregated Mode 15 repair statistics

- repair_considered_constraints_total: 0
- repair_selected_constraints_total: 0
- repair_skipped_by_topk_total: 0
- safe_accept_checked_total: 0
- safe_harmful_prevented_total: 0
- safe_rejected_new_sat_violation_total: 0
- safe_rejected_no_violation_gain_total: 0
- safe_rejected_total: 0

## Per-instance results

| Instance | Sense | Mode 15 | Original | Result | Exit (new/old) | Wall s (new/old) |
|---|---:|---:|---:|---|---:|---:|
| QPLIB_7149.lp | min | 959 | 959 | tie | 0/0 | 10.131/10.103 |
| QPLIB_2017.lp | min | -22984 | -22984 | tie | 0/0 | 10.075/10.089 |
| QPLIB_6757.lp | min | 175 | 175 | tie | 0/0 | 12.702/12.314 |
| QPLIB_2077.lp | min | 2443612.5 | 2443612.5 | tie | 0/0 | 10.196/10.801 |
| QPLIB_2047.lp | min | 1182604.5 | 1182604.5 | tie | 0/0 | 10.421/10.202 |
| QPLIB_3852.lp | max | 234 | 234 | tie | 0/0 | 10.080/10.026 |
| QPLIB_3865.lp | min | 6553485 | 6553485 | tie | 0/0 | 12.926/13.553 |
| chimera_k64maxcut-02.lp | max | 14.3 | 12.1 | new win | 0/0 | 10.151/10.215 |
| st_testgr3.lp | min | -19.4915 | -19.4915 | tie | 0/0 | 10.037/10.048 |
| QPLIB_3860.lp | min | -20161 | -20161 | tie | 0/0 | 10.454/10.786 |
| QPLIB_3751.lp | min | 2312 | 2312 | tie | 0/0 | 10.031/10.123 |
| graphpart_2pm-0044-0044.lp | min | -10 | -10 | tie | 0/0 | 10.012/10.010 |
| nvs24.lp | min | -1033.2 | -1033.2 | tie | 0/0 | 10.012/10.114 |
| chimera_mgw-c16-2031-02.lp | max | 1967 | 1967 | tie | 0/0 | 10.762/10.730 |
| QPLIB_9048.lp | min | -1.134365 | -1.165682 | new loss | 0/0 | 10.073/10.036 |
| QPLIB_3693.lp | max | 1144 | 1146 | new loss | 0/0 | 10.257/10.213 |
| QPLIB_2492.lp | min | 2766 | 2766 | tie | 0/0 | 10.061/10.138 |
| graphpart_2g-0077-0077.lp | min | -2831099 | -2831099 | tie | 0/0 | 10.023/10.009 |
| QPLIB_5721.lp | max | 7930221 | 7930221 | tie | 0/0 | 12.138/12.565 |
| QPLIB_3772.lp | min | -938 | -938 | tie | 0/0 | 10.176/10.170 |
| chimera_rfr-01.lp | max | 1166.4 | 1166.4 | tie | 0/0 | 10.841/10.992 |
| st_testgr1.lp | min | -12.38955 | -12.38955 | tie | 0/0 | 10.009/10.014 |
| QPLIB_2036.lp | min | -30570 | -30570 | tie | 0/0 | 10.100/10.057 |
| QPLIB_7139.lp | min | 621 | 621 | tie | 0/0 | 10.050/10.051 |
| QPLIB_5944.lp | max | 1326 | 1326 | tie | 0/0 | 10.222/10.333 |
| QPLIB_10040.lp | min | 0 | 0 | tie | 0/0 | 10.490/10.274 |
| QPLIB_3841.lp | min | -1817 | -1817 | tie | 0/0 | 10.528/10.481 |
| st_test3.lp | min | -7 | -7 | tie | 0/0 | 10.012/10.010 |
| QPLIB_3752.lp | min | -1217 | -1217 | tie | 0/0 | 10.066/10.174 |
| prob03.lp | min | 10 | 10 | tie | 0/0 | 10.010/10.008 |
| QPLIB_5909.lp | max | 35726 | 35726 | tie | 0/0 | 10.200/10.213 |
| QPLIB_5935.lp | max | 2588 | 2588 | tie | 0/0 | 10.211/10.316 |
| QPLIB_5875.lp | max | 43757 | 43757 | tie | 0/0 | 11.243/11.137 |
| QPLIB_3815.lp | min | -65 | -65 | tie | 0/0 | 10.027/10.010 |
| graphpart_2g-0099-9211.lp | min | -5018798 | -5018798 | tie | 0/0 | 10.036/10.014 |
| st_miqp2.lp | min | 2 | 2 | tie | 0/0 | 10.008/10.007 |
| QPLIB_0752.lp | max | 24071 | 24071 | tie | 0/0 | 10.218/10.077 |
| QPLIB_10045.lp | min | 0 | 0 | tie | 0/0 | 10.728/10.494 |
| QPLIB_3587.lp | min | 16811 | 16811 | tie | 0/0 | 10.107/10.044 |
| QPLIB_10071.lp | min | 0 | 0 | tie | 0/0 | 10.651/11.032 |
| graphpart_3g-0344-0344.lp | min | -4963154 | -4963154 | tie | 0/0 | 10.020/10.019 |
| QPLIB_3562.lp | min | 16.2 | 15.6 | new loss | 0/0 | 10.036/10.008 |
| QPLIB_3614.lp | min | 14418 | 14448 | new win | 0/0 | 10.055/10.032 |
| QPLIB_3780.lp | min | 103.2 | 106 | new win | 0/0 | 10.036/10.028 |
| QPLIB_10059.lp | min | 0 | 0 | tie | 0/0 | 10.095/10.481 |
| QPLIB_2096.lp | min | 7381576 | 7381576 | tie | 0/0 | 11.216/11.202 |
| chimera_mgw-c16-2031-01.lp | max | 1959 | 1959 | tie | 0/0 | 10.906/10.752 |
| QPLIB_2359.lp | min | -638 | -638 | tie | 0/0 | 10.092/10.039 |
| QPLIB_3745.lp | max | 334 | 334 | tie | 0/0 | 10.074/10.102 |
| graphpart_3pm-0334-0334.lp | min | -36 | -36 | tie | 0/0 | 10.010/10.017 |
