#!/usr/bin/env python3
import csv
import math
import re
import sys
from pathlib import Path


RESULT_RE = re.compile(
    r"best obj (min|max)\s*=\s*([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)"
)
FLOAT_FIELDS = {
    "repair_considered_constraints_total",
    "repair_selected_constraints_total",
    "repair_skipped_by_topk_total",
    "safe_accept_checked_total",
    "safe_rejected_total",
    "safe_rejected_no_violation_gain_total",
    "safe_rejected_new_sat_violation_total",
    "safe_harmful_prevented_total",
    "serial_diversification_total",
    "qubo_gain_cache_hits_total",
    "qubo_gain_cache_rebuild_total",
    "qubo_gain_cache_neighbor_updates_total",
    "qubo_gain_cache_mismatch_total",
}


def parse_log(path: Path):
    text = path.read_text(errors="replace") if path.exists() else ""
    matches = list(RESULT_RE.finditer(text))
    sense = matches[-1].group(1) if matches else ""
    objective = float(matches[-1].group(2)) if matches else None
    if ("solution_status = NO_FEASIBLE_SOLUTION" in text
            or objective in (2147483647.0, 9223372036854775807.0)):
        objective = None

    def last_value(key, cast, default):
        # Metadata can be attached to an unterminated final solver output line.
        values = re.findall(rf"{re.escape(key)}\s*=\s*(\S+)", text)
        if not values:
            return default
        try:
            return cast(values[-1])
        except ValueError:
            return default

    stats = {key: last_value(key, int, 0) for key in FLOAT_FIELDS}
    return {
        "sense": sense,
        "objective": objective,
        "exit_code": last_value("benchmark_exit_code", int, -1),
        "wall_time": last_value("wall_time_seconds", float, math.nan),
        "stats": stats,
    }


def display(value):
    return "N/A" if value is None else f"{value:.12g}"


def compare(new, old):
    new_obj = new["objective"]
    old_obj = old["objective"]
    if new_obj is None and old_obj is None:
        return "neither feasible"
    if new_obj is not None and old_obj is None:
        return "new only feasible"
    if new_obj is None and old_obj is not None:
        return "old only feasible"
    if new["sense"] != old["sense"] or not new["sense"]:
        return "sense mismatch"

    tolerance = 1e-9 * (1.0 + max(abs(new_obj), abs(old_obj)))
    delta = new_obj - old_obj
    if abs(delta) <= tolerance:
        return "tie"
    if new["sense"] == "min":
        return "new win" if delta < 0 else "new loss"
    return "new win" if delta > 0 else "new loss"


def main():
    results_dir = Path(sys.argv[1])
    status_text = (results_dir / "status.txt").read_text(errors="replace")
    mode_match = re.search(r"mode(?:=|:)(\d+)", status_text)
    mode_label = f"Mode {mode_match.group(1)}" if mode_match else "Current solver"
    cutoff_match = re.search(r"cutoff_seconds=(\d+)", status_text)
    cutoff_label = cutoff_match.group(1) if cutoff_match else "10"
    instances = [
        line.strip()
        for line in (results_dir / "selected_instances.txt").read_text().splitlines()
        if line.strip()
    ]

    rows = []
    aggregate_stats = {key: 0 for key in FLOAT_FIELDS}
    for inst in instances:
        new = parse_log(results_dir / "logs" / f"{inst}.new.log")
        old = parse_log(results_dir / "logs" / f"{inst}.old.log")
        outcome = compare(new, old)
        for key, value in new["stats"].items():
            aggregate_stats[key] += value
        rows.append((inst, new, old, outcome))

    with (results_dir / "comparison.csv").open("w", newline="") as output:
        writer = csv.writer(output)
        writer.writerow([
            "instance", "sense", "new_objective", "old_objective", "outcome",
            "new_exit_code", "old_exit_code", "new_wall_seconds", "old_wall_seconds",
        ])
        for inst, new, old, outcome in rows:
            writer.writerow([
                inst, new["sense"] or old["sense"], new["objective"], old["objective"], outcome,
                new["exit_code"], old["exit_code"], new["wall_time"], old["wall_time"],
            ])

    counts = {}
    for _, _, _, outcome in rows:
        counts[outcome] = counts.get(outcome, 0) + 1
    new_feasible = sum(new["objective"] is not None for _, new, _, _ in rows)
    old_feasible = sum(old["objective"] is not None for _, _, old, _ in rows)
    new_nonzero = sum(new["exit_code"] != 0 for _, new, _, _ in rows)
    old_nonzero = sum(old["exit_code"] != 0 for _, _, old, _ in rows)

    with (results_dir / "summary.md").open("w") as output:
        output.write(f"# {mode_label} vs Original LS-IQCQP ({len(rows)} instances, {cutoff_label}s)\n\n")
        output.write(f"- {mode_label} feasible: {new_feasible}/{len(rows)}\n")
        output.write(f"- Original feasible: {old_feasible}/{len(rows)}\n")
        output.write(f"- {mode_label} wins / ties / losses: {counts.get('new win', 0)} / {counts.get('tie', 0)} / {counts.get('new loss', 0)}\n")
        output.write(f"- {mode_label} only feasible: {counts.get('new only feasible', 0)}\n")
        output.write(f"- Original only feasible: {counts.get('old only feasible', 0)}\n")
        output.write(f"- Neither feasible: {counts.get('neither feasible', 0)}\n")
        output.write(f"- Nonzero exits ({mode_label} / original): {new_nonzero} / {old_nonzero}\n\n")
        output.write(f"## Aggregated {mode_label} repair statistics\n\n")
        for key in sorted(aggregate_stats):
            output.write(f"- {key}: {aggregate_stats[key]}\n")
        output.write("\n## Per-instance results\n\n")
        output.write(f"| Instance | Sense | {mode_label} | Original | Result | Exit (new/old) | Wall s (new/old) |\n")
        output.write("|---|---:|---:|---:|---|---:|---:|\n")
        for inst, new, old, outcome in rows:
            sense = new["sense"] or old["sense"] or "N/A"
            output.write(
                f"| {inst} | {sense} | {display(new['objective'])} | {display(old['objective'])} "
                f"| {outcome} | {new['exit_code']}/{old['exit_code']} "
                f"| {new['wall_time']:.3f}/{old['wall_time']:.3f} |\n"
            )


if __name__ == "__main__":
    main()
