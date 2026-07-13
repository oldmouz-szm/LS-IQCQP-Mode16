#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP"
DATA="$ROOT/data/all_lp"
CURRENT="$ROOT/src/LS-IQCQP/build/LS-IQCQP"
BASE="/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP"
OUT_ROOT="$ROOT/results/mode16_pairflip_nohup"
PARALLEL="${PARALLEL:-24}"

mkdir -p "$OUT_ROOT"
find "$DATA" -maxdepth 1 -type f -name '*.lp' -printf '%f\n' | sort > "$OUT_ROOT/instances.txt"

run_task() {
    local cutoff="$1"
    local mode="$2"
    local inst="$3"
    local cutoff_dir="$OUT_ROOT/${cutoff}s"
    local stem="${inst%.lp}"
    local out="$cutoff_dir/${stem}.${mode}.out"
    local err="$cutoff_dir/${stem}.${mode}.err"

    if [[ -s "$out" ]]; then
        return 0
    fi

    case "$mode" in
        baseline)
            "$BASE" "$cutoff" 1 "$DATA/$inst" > "$out" 2> "$err"
            ;;
        mode15)
            "$CURRENT" "$cutoff" 1 "$DATA/$inst" 1 15 3 8 > "$out" 2> "$err"
            ;;
        mode16)
            "$CURRENT" "$cutoff" 1 "$DATA/$inst" 1 16 3 8 > "$out" 2> "$err"
            ;;
        *)
            echo "unknown mode: $mode" >&2
            return 2
            ;;
    esac
}
export ROOT DATA CURRENT BASE OUT_ROOT
export -f run_task

summarize_cutoff() {
    local cutoff="$1"
    python3 - "$OUT_ROOT" "$cutoff" <<'PY'
from pathlib import Path
import csv
import re
import sys

out_root = Path(sys.argv[1])
cutoff = sys.argv[2]
rd = out_root / f"{cutoff}s"
instances = (out_root / "instances.txt").read_text().splitlines()

def read(path):
    return path.read_text(errors="ignore") if path.exists() else ""

def parse(stem, mode):
    txt = read(rd / f"{stem}.{mode}.out") + "\n" + read(rd / f"{stem}.{mode}.err")
    status = None
    m = re.search(r"solution_status\s*=\s*(\S+)", txt)
    if m:
        status = m.group(1)
    obj = None
    sense = None
    m = re.search(r"best obj (min|max)=\s*([-+0-9.eE]+)", txt)
    if m:
        sense = m.group(1)
        obj = float(m.group(2))
    if status is None and obj is not None:
        status = "FEASIBLE"
    if status is None and "NO_FEASIBLE_SOLUTION" in txt:
        status = "NO_FEASIBLE_SOLUTION"
    stats = {}
    for k in [
        "qubo_pair_flip_attempt_total",
        "qubo_pair_flip_checked_total",
        "qubo_pair_flip_executed_total",
        "qubo_pair_flip_improved_best_total",
        "qubo_pair_flip_best_score_seen",
        "qubo_pair_flip_time_spent",
        "qubo_gain_cache_mismatch_total",
        "qubo_gain_cache_active",
        "serial_diversification_total",
        "search_steps",
    ]:
        mm = re.search(rf"{k}\s*=\s*([-+0-9.eE]+)", txt)
        stats[k] = float(mm.group(1)) if mm else 0.0
    return {"obj": obj, "sense": sense, "status": status or "UNKNOWN", **stats}

def compare(a, b):
    if a["obj"] is not None and b["obj"] is None:
        return "win"
    if a["obj"] is None and b["obj"] is not None:
        return "loss"
    if a["obj"] is None and b["obj"] is None:
        return "tie"
    sense = a["sense"] or b["sense"] or "min"
    if sense == "max":
        if a["obj"] > b["obj"] + 1e-6:
            return "win"
        if a["obj"] < b["obj"] - 1e-6:
            return "loss"
    else:
        if a["obj"] < b["obj"] - 1e-6:
            return "win"
        if a["obj"] > b["obj"] + 1e-6:
            return "loss"
    return "tie"

rows = []
for inst in instances:
    stem = inst[:-3]
    baseline = parse(stem, "baseline")
    mode15 = parse(stem, "mode15")
    mode16 = parse(stem, "mode16")
    rows.append({
        "instance": inst,
        "family": inst.split("_")[0].split("-")[0],
        "is_chimera": int(inst.startswith("chimera")),
        "is_qubo_cache_active": int(mode16["qubo_gain_cache_active"] == 1),
        "sense": mode16["sense"] or mode15["sense"] or baseline["sense"] or "",
        "baseline_status": baseline["status"],
        "mode15_status": mode15["status"],
        "mode16_status": mode16["status"],
        "baseline_obj": baseline["obj"],
        "mode15_obj": mode15["obj"],
        "mode16_obj": mode16["obj"],
        "mode16_vs_baseline": compare(mode16, baseline),
        "mode16_vs_mode15": compare(mode16, mode15),
        "pair_attempt": int(mode16["qubo_pair_flip_attempt_total"]),
        "pair_checked": int(mode16["qubo_pair_flip_checked_total"]),
        "pair_executed": int(mode16["qubo_pair_flip_executed_total"]),
        "pair_improved_best": int(mode16["qubo_pair_flip_improved_best_total"]),
        "pair_best_seen": mode16["qubo_pair_flip_best_score_seen"],
        "pair_time_spent": mode16["qubo_pair_flip_time_spent"],
        "cache_mismatch16": int(mode16["qubo_gain_cache_mismatch_total"]),
        "mode15_cache_mismatch": int(mode15["qubo_gain_cache_mismatch_total"]),
    })

with (rd / "summary.csv").open("w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
    writer.writeheader()
    writer.writerows(rows)

def aggregate(name, pred):
    sub = [r for r in rows if pred(r)]
    def counts(key):
        return {x: sum(1 for r in sub if r[key] == x) for x in ["win", "tie", "loss"]}
    cb = counts("mode16_vs_baseline")
    cm = counts("mode16_vs_mode15")
    return {
        "subset": name,
        "instances": len(sub),
        "mode16_vs_baseline_win": cb["win"],
        "mode16_vs_baseline_tie": cb["tie"],
        "mode16_vs_baseline_loss": cb["loss"],
        "mode16_vs_mode15_win": cm["win"],
        "mode16_vs_mode15_tie": cm["tie"],
        "mode16_vs_mode15_loss": cm["loss"],
        "mode16_feasible_or_obj": sum(1 for r in sub if r["mode16_obj"] not in ("", None)),
        "baseline_feasible_or_obj": sum(1 for r in sub if r["baseline_obj"] not in ("", None)),
        "mode15_feasible_or_obj": sum(1 for r in sub if r["mode15_obj"] not in ("", None)),
        "pair_attempt_sum": sum(r["pair_attempt"] for r in sub),
        "pair_checked_sum": sum(r["pair_checked"] for r in sub),
        "pair_executed_sum": sum(r["pair_executed"] for r in sub),
        "pair_improved_best_sum": sum(r["pair_improved_best"] for r in sub),
        "pair_time_spent_sum": sum(r["pair_time_spent"] for r in sub),
        "cache_mismatch16_sum": sum(r["cache_mismatch16"] for r in sub),
        "mode15_cache_mismatch_sum": sum(r["mode15_cache_mismatch"] for r in sub),
    }

aggs = [
    aggregate("all", lambda r: True),
    aggregate("qubo_cache_active", lambda r: bool(r["is_qubo_cache_active"])),
    aggregate("chimera", lambda r: bool(r["is_chimera"])),
    aggregate("chimera_qubo_cache_active", lambda r: bool(r["is_chimera"]) and bool(r["is_qubo_cache_active"])),
]

with (rd / "aggregate.csv").open("w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=list(aggs[0].keys()))
    writer.writeheader()
    writer.writerows(aggs)

def loss_gap(row):
    if row["mode16_obj"] is None or row["mode15_obj"] is None:
        return 0
    if row["sense"] == "max":
        return row["mode15_obj"] - row["mode16_obj"]
    return row["mode16_obj"] - row["mode15_obj"]

losses = sorted([r for r in rows if r["mode16_vs_mode15"] == "loss"], key=loss_gap, reverse=True)
lines = [f"# Mode 16 Pair Flip {cutoff}s Report", ""]
lines.append(f"- Instances: {len(rows)}")
for a in aggs:
    lines.append(
        f"- {a['subset']}: Mode16 vs baseline "
        f"{a['mode16_vs_baseline_win']}/{a['mode16_vs_baseline_tie']}/{a['mode16_vs_baseline_loss']}, "
        f"Mode16 vs Mode15 {a['mode16_vs_mode15_win']}/{a['mode16_vs_mode15_tie']}/{a['mode16_vs_mode15_loss']}"
    )
lines += ["", "## Pair Flip And Cache"]
a = aggs[0]
lines.append(f"- Pair attempts/checked/executed: {a['pair_attempt_sum']}/{a['pair_checked_sum']}/{a['pair_executed_sum']}")
lines.append(f"- Pair improved-best count: {a['pair_improved_best_sum']}")
lines.append(f"- Pair time spent sum: {a['pair_time_spent_sum']:.3f}s")
lines.append(f"- Mode16 cache mismatch sum: {a['cache_mismatch16_sum']}")
lines.append(f"- Mode15 cache mismatch sum: {a['mode15_cache_mismatch_sum']}")
lines += ["", "## Largest Mode16 vs Mode15 Losses"]
for r in losses[:10]:
    lines.append(
        f"- {r['instance']}: mode15={r['mode15_obj']}, mode16={r['mode16_obj']}, "
        f"sense={r['sense']}, pair_exec={r['pair_executed']}"
    )
lines += ["", "## Files", "- summary.csv", "- aggregate.csv"]
(rd / "report.md").write_text("\n".join(lines) + "\n")

print((rd / "aggregate.csv").read_text())
PY
}

for cutoff in 30 300; do
    cutoff_dir="$OUT_ROOT/${cutoff}s"
    mkdir -p "$cutoff_dir"
    echo "[$(date '+%F %T')] start cutoff=${cutoff}s parallel=$PARALLEL"
    : > "$cutoff_dir/tasks.tsv"
    while IFS= read -r inst; do
        printf '%s\t%s\t%s\n' "$cutoff" baseline "$inst" >> "$cutoff_dir/tasks.tsv"
        printf '%s\t%s\t%s\n' "$cutoff" mode15 "$inst" >> "$cutoff_dir/tasks.tsv"
        printf '%s\t%s\t%s\n' "$cutoff" mode16 "$inst" >> "$cutoff_dir/tasks.tsv"
    done < "$OUT_ROOT/instances.txt"
    cat "$cutoff_dir/tasks.tsv" | xargs -P "$PARALLEL" -n 3 bash -lc 'run_task "$0" "$1" "$2"'
    echo "[$(date '+%F %T')] summarize cutoff=${cutoff}s"
    summarize_cutoff "$cutoff"
done

python3 - "$OUT_ROOT" <<'PY'
from pathlib import Path
import csv
import sys

out_root = Path(sys.argv[1])
lines = ["# Mode 16 Pair Flip Nohup Overview", ""]
for cutoff in ["30", "300"]:
    agg_path = out_root / f"{cutoff}s" / "aggregate.csv"
    if not agg_path.exists():
        continue
    rows = list(csv.DictReader(agg_path.open()))
    lines.append(f"## {cutoff}s")
    for row in rows:
        lines.append(
            f"- {row['subset']}: vs baseline "
            f"{row['mode16_vs_baseline_win']}/{row['mode16_vs_baseline_tie']}/{row['mode16_vs_baseline_loss']}, "
            f"vs Mode15 {row['mode16_vs_mode15_win']}/{row['mode16_vs_mode15_tie']}/{row['mode16_vs_mode15_loss']}, "
            f"pair executed {row['pair_executed_sum']}, mismatch {row['cache_mismatch16_sum']}"
        )
    lines.append("")
(out_root / "report_overview.md").write_text("\n".join(lines))
PY

echo "[$(date '+%F %T')] all done"
