# Block Move Ablation Report

## 1. Setup
- **Project Path**: `/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP`
- **Instances**: `/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp`

## 2. Run Status
- Completed Runs: 5700
- Parse Errors: 0
- Consistency Errors: 0

## 3. Feasibility Results
Mode | Feasible | Total | Rate
---|---|---|---
old_block | 900 | 900 | 100.00%
smart_only | 900 | 900 | 100.00%
improve_only | 720 | 900 | 80.00%
baseline | 240 | 300 | 80.00%
repair_only | 900 | 900 | 100.00%
full_block | 900 | 900 | 100.00%
normalized_only | 900 | 900 | 100.00%

## 4. Objective Results
Comparison | Win | Tie | Loss | Unsolved
---|---|---|---|---
full_block vs baseline | 74 | 211 | 15 | 0
repair_only vs baseline | 74 | 218 | 8 | 0
improve_only vs baseline | 13 | 213 | 14 | 60
smart_only vs old_block | 39 | 236 | 25 | 0
normalized_only vs old_block | 23 | 237 | 40 | 0
full_block vs repair_only | 29 | 247 | 24 | 0

## 5. Ablation Study
- **Smart vs Old**: Review `table_win_loss.csv` for smart_only vs old_block.
- **Repair vs Full**: Review feasibility vs best objective trade-offs.
- **Block Size**: Check `table_block_size.csv` for the optimal size.

## 6. Cost Analysis
- Check `table_block_size.csv` for avg_wall_time impact.

## 7. Findings
*(Auto-generated placeholder - Block move's main contribution is often feasibility repair. Verify if objective stabilizes.)*

## 8. Next Steps
- Review figures in `figures/` directory.
- Note: 当前结论只基于项目样例目录 data/all_lp 的测试，不能直接代表完整 QPLIB/MINLPLIB。
