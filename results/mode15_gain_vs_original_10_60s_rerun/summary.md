# Mode 15 vs Original LS-IQCQP (10 instances, 60s)

- Mode 15 feasible: 10/10
- Original feasible: 10/10
- Mode 15 wins / ties / losses: 5 / 4 / 1
- Mode 15 only feasible: 0
- Original only feasible: 0
- Neither feasible: 0
- Nonzero exits (Mode 15 / original): 0 / 0

## Aggregated Mode 15 repair statistics

- qubo_gain_cache_hits_total: 70026307
- qubo_gain_cache_mismatch_total: 0
- qubo_gain_cache_neighbor_updates_total: 11921775
- qubo_gain_cache_rebuild_total: 39
- repair_considered_constraints_total: 0
- repair_selected_constraints_total: 0
- repair_skipped_by_topk_total: 0
- safe_accept_checked_total: 0
- safe_harmful_prevented_total: 0
- safe_rejected_new_sat_violation_total: 0
- safe_rejected_no_violation_gain_total: 0
- safe_rejected_total: 0
- serial_diversification_total: 15

## Per-instance results

| Instance | Sense | Mode 15 | Original | Result | Exit (new/old) | Wall s (new/old) |
|---|---:|---:|---:|---|---:|---:|
| QPLIB_3587.lp | min | 16725 | 16725 | tie | 0/0 | 60.022/60.025 |
| QPLIB_3650.lp | max | 920 | 920 | tie | 0/0 | 60.035/60.051 |
| chimera_selby-c8-onc8-01.lp | max | 176.1 | 176.5 | new loss | 0/0 | 60.033/60.086 |
| chimera_selby-c16-02.lp | max | 714.8 | 710.1 | new win | 0/0 | 60.262/60.480 |
| QPLIB_5755.lp | max | 24205342 | 24046773 | new win | 0/0 | 60.087/60.075 |
| chimera_k64ising-01.lp | max | 18.8 | 12.1 | new win | 0/0 | 60.170/60.029 |
| QPLIB_3850.lp | max | 1196 | 1194 | new win | 0/0 | 60.190/60.161 |
| QPLIB_3693.lp | max | 1148 | 1148 | tie | 0/0 | 60.230/60.299 |
| chimera_lga-02.lp | max | 127.4 | 125.6 | new win | 0/0 | 60.023/60.172 |
| QPLIB_9048.lp | min | -1.165682 | -1.165682 | tie | 0/0 | 60.978/60.143 |
