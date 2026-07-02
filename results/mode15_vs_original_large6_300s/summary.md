# Mode 15 vs Original LS-IQCQP (6 instances, 300s)

- Mode 15 feasible: 5/6
- Original feasible: 5/6
- Mode 15 wins / ties / losses: 2 / 2 / 1
- Mode 15 only feasible: 0
- Original only feasible: 0
- Neither feasible: 1
- Nonzero exits (Mode 15 / original): 0 / 0

## Aggregated Mode 15 repair statistics

- qubo_gain_cache_hits_total: 31857109
- qubo_gain_cache_mismatch_total: 0
- qubo_gain_cache_neighbor_updates_total: 9103773
- qubo_gain_cache_rebuild_total: 16
- repair_considered_constraints_total: 0
- repair_selected_constraints_total: 0
- repair_skipped_by_topk_total: 0
- safe_accept_checked_total: 0
- safe_harmful_prevented_total: 0
- safe_rejected_new_sat_violation_total: 0
- safe_rejected_no_violation_gain_total: 0
- safe_rejected_total: 0
- serial_diversification_total: 0

## Per-instance results

| Instance | Sense | Mode 15 | Original | Result | Exit (new/old) | Wall s (new/old) |
|---|---:|---:|---:|---|---:|---:|
| QPLIB_9030.lp | min | 526240 | 526240 | tie | 0/0 | 307.787/306.024 |
| QPLIB_3380.lp | min | N/A | N/A | neither feasible | 0/0 | 300.340/300.385 |
| QPLIB_6941.lp | min | 193 | 193 | tie | 0/0 | 304.234/304.151 |
| chimera_mgw-c16-2031-01.lp | max | 2002 | 2001 | new win | 0/0 | 300.181/300.065 |
| chimera_rfr-01.lp | max | 1192.7 | 1187.9 | new win | 0/0 | 300.188/300.623 |
| chimera_selby-c16-02.lp | max | 718.6 | 719.4 | new loss | 0/0 | 300.348/300.273 |
