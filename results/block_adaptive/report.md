# Phase F Adaptive Block Trigger Evaluation

## Head-to-Head Comparison (10s Cutoff)

- **full_block vs baseline**: Wins: 0, Losses: 1, Ties: 0 (M1 failed: 0, M2 failed: 0, Both failed: 3)
- **mode_6_adaptive_repair vs baseline**: Wins: 0, Losses: 1, Ties: 0 (M1 failed: 1, M2 failed: 0, Both failed: 3)
- **mode_6_adaptive_repair vs full_block**: Wins: 0, Losses: 1, Ties: 0 (M1 failed: 1, M2 failed: 0, Both failed: 3)
- **mode_7_adaptive_both vs full_block**: Wins: 0, Losses: 1, Ties: 0 (M1 failed: 1, M2 failed: 0, Both failed: 3)
- **mode_6_adaptive_repair vs mode_7_adaptive_both**: Wins: 0, Losses: 0, Ties: 0 (M1 failed: 0, M2 failed: 0, Both failed: 4)
- **mode_9_legacy_safe vs baseline**: Wins: 0, Losses: 1, Ties: 0 (M1 failed: 1, M2 failed: 0, Both failed: 3)

## Block Overhead & Metrics (10s Cutoff)

| Mode | Avg Block Executed | Avg Block Time (s) | Avg Repair Success | Avg Unconstrained Disabled |
|---|---|---|---|---|
| full_block | 1659.75 | 1.6157 | 0.00 | 0.00 |
| mode_6_adaptive_repair | 0.00 | 0.0000 | 0.00 | 0.00 |
| mode_7_adaptive_both | 0.00 | 0.0000 | 0.00 | 0.00 |
| mode_9_legacy_safe | 0.00 | 0.0000 | 0.00 | 0.00 |
