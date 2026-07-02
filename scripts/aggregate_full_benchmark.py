#!/usr/bin/env python3
import csv
import sys
from collections import Counter
from pathlib import Path


def main():
    root = Path(sys.argv[1])
    all_rows = []
    summaries = []

    for cutoff in (10, 60, 300):
        path = root / f"{cutoff}s" / "comparison.csv"
        rows = list(csv.DictReader(path.open()))
        counts = Counter(row["outcome"] for row in rows)
        new_feasible = sum(row["new_objective"] not in ("", None) for row in rows)
        old_feasible = sum(row["old_objective"] not in ("", None) for row in rows)
        new_nonzero = sum(int(row["new_exit_code"]) != 0 for row in rows)
        old_nonzero = sum(int(row["old_exit_code"]) != 0 for row in rows)
        summaries.append({
            "cutoff": cutoff,
            "instances": len(rows),
            "new_feasible": new_feasible,
            "old_feasible": old_feasible,
            "wins": counts["new win"],
            "ties": counts["tie"],
            "losses": counts["new loss"],
            "new_only_feasible": counts["new only feasible"],
            "old_only_feasible": counts["old only feasible"],
            "neither_feasible": counts["neither feasible"],
            "sense_mismatch": counts["sense mismatch"],
            "new_nonzero_exit": new_nonzero,
            "old_nonzero_exit": old_nonzero,
        })
        for row in rows:
            all_rows.append({"cutoff": cutoff, **row})

    with (root / "summary_by_cutoff.csv").open("w", newline="") as output:
        writer = csv.DictWriter(output, fieldnames=summaries[0].keys())
        writer.writeheader()
        writer.writerows(summaries)

    with (root / "all_comparisons.csv").open("w", newline="") as output:
        writer = csv.DictWriter(output, fieldnames=all_rows[0].keys())
        writer.writeheader()
        writer.writerows(all_rows)

    with (root / "summary.md").open("w") as output:
        output.write("# Mode 15 vs Original LS-IQCQP — Full 221-Instance Benchmark\n\n")
        output.write("| Cutoff | Feasible (Mode 15 / Original) | Wins | Ties | Losses | New-only feasible | Old-only feasible | Neither feasible | Nonzero exits (new/old) |\n")
        output.write("|---:|---:|---:|---:|---:|---:|---:|---:|---:|\n")
        for row in summaries:
            output.write(
                f"| {row['cutoff']}s | {row['new_feasible']} / {row['old_feasible']} "
                f"| {row['wins']} | {row['ties']} | {row['losses']} "
                f"| {row['new_only_feasible']} | {row['old_only_feasible']} "
                f"| {row['neither_feasible']} "
                f"| {row['new_nonzero_exit']} / {row['old_nonzero_exit']} |\n"
            )

        output.write("\n## Aggregate across all cutoffs\n\n")
        output.write(f"- Wins: {sum(row['wins'] for row in summaries)}\n")
        output.write(f"- Ties: {sum(row['ties'] for row in summaries)}\n")
        output.write(f"- Losses: {sum(row['losses'] for row in summaries)}\n")
        output.write(f"- Mode 15-only feasible: {sum(row['new_only_feasible'] for row in summaries)}\n")
        output.write(f"- Original-only feasible: {sum(row['old_only_feasible'] for row in summaries)}\n")


if __name__ == "__main__":
    main()

