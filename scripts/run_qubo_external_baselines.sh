#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP"
CUTOFF="${1:-10}"
OUT="$ROOT/results/qubo_external_baselines_${CUTOFF}s"
PARALLEL="${PARALLEL:-4}"

mkdir -p "$OUT/parts"
python3 - "$ROOT" "$OUT/instances.txt" <<'PY'
import csv
import sys
from pathlib import Path

root = Path(sys.argv[1])
out = Path(sys.argv[2])
rows = list(csv.DictReader((root / "results/mode16_pairflip_10s/summary.csv").open()))
instances = [r["instance"] for r in rows if r["is_qubo_cache_active"] == "1"]
out.write_text("\n".join(instances) + "\n")
print(len(instances))
PY

run_one() {
    local inst="$1"
    local stem="${inst%.lp}"
    local part="$OUT/parts/$stem"
    mkdir -p "$part"
    python3 "$ROOT/scripts/qubo_baseline_compare.py" \
        --instances "$inst" \
        --cutoff "$CUTOFF" \
        --out-dir "$part" \
        --solvers mode16 dwave_tabu neal_sa gurobi \
        > "$part/run.log" 2>&1
}
export ROOT CUTOFF OUT
export -f run_one

cat "$OUT/instances.txt" | xargs -n 1 -P "$PARALLEL" bash -lc 'run_one "$0"'

python3 - "$OUT" <<'PY'
import csv
import sys
from pathlib import Path

out = Path(sys.argv[1])
rows = []
for path in sorted((out / "parts").glob("*/raw_results.csv")):
    rows.extend(csv.DictReader(path.open()))

if not rows:
    raise SystemExit("no rows")

fields = list(rows[0].keys())
with (out / "raw_results.csv").open("w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=fields)
    w.writeheader()
    w.writerows(rows)

summary = []
for solver in sorted({r["solver"] for r in rows if r["solver"] != "mode16"}):
    sub = [r for r in rows if r["solver"] == solver]
    summary.append({
        "solver": solver,
        "instances": len(sub),
        "mode16_win": sum(1 for r in sub if r["mode16_vs_solver"] == "win"),
        "tie": sum(1 for r in sub if r["mode16_vs_solver"] == "tie"),
        "mode16_loss": sum(1 for r in sub if r["mode16_vs_solver"] == "loss"),
        "solver_errors": sum(1 for r in sub if str(r["status"]).startswith("error")),
    })

with (out / "summary.csv").open("w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(summary[0].keys()))
    w.writeheader()
    w.writerows(summary)

lines = [f"# External QUBO Baselines {out.name}", ""]
for s in summary:
    lines.append(f"- Mode16 vs {s['solver']}: {s['mode16_win']}/{s['tie']}/{s['mode16_loss']} errors={s['solver_errors']}")
lines.append("")
lines.append("## Largest Mode16 Losses")
def gap(row):
    try:
        a = float(row["mode16_obj"])
        b = float(row["reported_obj"])
    except Exception:
        return 0.0
    if row["sense"] == "max":
        return b - a
    return a - b
losses = [r for r in rows if r["solver"] != "mode16" and r["mode16_vs_solver"] == "loss"]
for r in sorted(losses, key=gap, reverse=True)[:20]:
    lines.append(f"- {r['instance']} {r['solver']}: mode16={r['mode16_obj']}, solver={r['reported_obj']}, sense={r['sense']}, status={r['status']}")
(out / "report.md").write_text("\n".join(lines) + "\n")

print((out / "summary.csv").read_text())
PY
