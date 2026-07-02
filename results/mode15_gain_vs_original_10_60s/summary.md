# Mode 15 vs Original LS-IQCQP (10 instances, 60s)

- Mode 15 feasible: 0/10
- Original feasible: 10/10
- Mode 15 wins / ties / losses: 0 / 0 / 0
- Mode 15 only feasible: 0
- Original only feasible: 10
- Neither feasible: 0
- Nonzero exits (Mode 15 / original): 10 / 0

## Aggregated Mode 15 repair statistics

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
| QPLIB_3587.lp | min | N/A | 16725 | old only feasible | 124/0 | 30.007/60.025 |
| QPLIB_3650.lp | max | N/A | 920 | old only feasible | 124/0 | 30.007/60.051 |
| chimera_selby-c8-onc8-01.lp | max | N/A | 176.5 | old only feasible | 124/0 | 30.007/60.086 |
| chimera_selby-c16-02.lp | max | N/A | 710.1 | old only feasible | 124/0 | 30.008/60.480 |
| QPLIB_5755.lp | max | N/A | 24046773 | old only feasible | 124/0 | 30.006/60.075 |
| chimera_k64ising-01.lp | max | N/A | 12.1 | old only feasible | 124/0 | 30.007/60.029 |
| QPLIB_3850.lp | max | N/A | 1194 | old only feasible | 124/0 | 30.006/60.161 |
| QPLIB_3693.lp | max | N/A | 1148 | old only feasible | 124/0 | 30.007/60.299 |
| chimera_lga-02.lp | max | N/A | 125.6 | old only feasible | 124/0 | 30.008/60.172 |
| QPLIB_9048.lp | min | N/A | -1.165682 | old only feasible | 124/0 | 30.007/60.143 |
