# Mode 15 vs Original LS-IQCQP (10 instances, 60s)

- Mode 15 feasible: 10/10
- Original feasible: 10/10
- Mode 15 wins / ties / losses: 2 / 7 / 1
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
- serial_diversification_total: 5

## Per-instance results

| Instance | Sense | Mode 15 | Original | Result | Exit (new/old) | Wall s (new/old) |
|---|---:|---:|---:|---|---:|---:|
| QPLIB_3587.lp | min | 16725 | 16725 | tie | 0/0 | 60.023/60.025 |
| QPLIB_3650.lp | max | 918 | 920 | new loss | 0/0 | 60.082/60.051 |
| chimera_selby-c8-onc8-01.lp | max | 176.5 | 176.5 | tie | 0/0 | 60.078/60.086 |
| chimera_selby-c16-02.lp | max | 711 | 710.1 | new win | 0/0 | 60.594/60.480 |
| QPLIB_5755.lp | max | 24177218 | 24046773 | new win | 0/0 | 60.029/60.075 |
| chimera_k64ising-01.lp | max | 12.1 | 12.1 | tie | 0/0 | 60.044/60.029 |
| QPLIB_3850.lp | max | 1194 | 1194 | tie | 0/0 | 60.261/60.161 |
| QPLIB_3693.lp | max | 1148 | 1148 | tie | 0/0 | 60.043/60.299 |
| chimera_lga-02.lp | max | 125.6 | 125.6 | tie | 0/0 | 60.419/60.172 |
| QPLIB_9048.lp | min | -1.165682 | -1.165682 | tie | 0/0 | 60.499/60.143 |
