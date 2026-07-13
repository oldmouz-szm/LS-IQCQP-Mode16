#!/usr/bin/env python3
import argparse
import csv
import math
import re
import subprocess
import time
from collections import defaultdict
from pathlib import Path


ROOT = Path("/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP")
DATA = ROOT / "data" / "all_lp"
MODE16_BIN = ROOT / "src" / "LS-IQCQP" / "build" / "LS-IQCQP"


def token_value(tok):
    if tok.startswith("."):
        tok = "0" + tok
    if tok.startswith("-."):
        tok = tok.replace("-.", "-0.", 1)
    return float(tok)


def split_lp_tokens(text):
    text = text.replace("[", " [ ").replace("]", " ] ")
    text = text.replace("*", " * ")
    return text.split()


def objective_section(text):
    lines = []
    in_obj = False
    sense = None
    for raw in text.splitlines():
        line = raw.strip()
        if not line or line.startswith("\\"):
            continue
        low = line.lower()
        if low == "maximize":
            in_obj = True
            sense = "max"
            continue
        if low == "minimize":
            in_obj = True
            sense = "min"
            continue
        if in_obj and low.startswith(("subject to", "such that", "st", "s.t.", "bounds", "binary", "binaries", "general", "end")):
            break
        if in_obj:
            lines.append(line)
    if sense is None:
        raise ValueError("missing objective sense")
    return sense, " ".join(lines)


def parse_lp_qubo(path):
    text = path.read_text(errors="ignore")
    sense, obj = objective_section(text)
    is_minimize = sense == "min"
    tokens = split_lp_tokens(obj)
    linear = defaultdict(float)
    quadratic = defaultdict(float)
    offset = 0.0
    variables = set()
    pos = True
    stage = 0
    coeff = None
    with_quadratic = False
    i = 0
    while i < len(tokens):
        tok = tokens[i]
        if tok == "obj:":
            i += 1
            continue
        if tok == "[":
            with_quadratic = True
            stage = 0
            pos = True
            i += 1
            continue
        if tok == "]":
            with_quadratic = False
            stage = 0
            pos = True
            i += 1
            continue
        if tok in ("/2", "/"):
            break
        if stage == 0:
            if tok == "+":
                pos = True
                stage = 1
                i += 1
                continue
            if tok == "-":
                pos = False
                stage = 1
                i += 1
                continue
            pos = True
            stage = 1
            continue
        if stage == 1:
            if re.match(r"^[A-Za-z_]", tok):
                coeff = 1.0 if pos else -1.0
                stage = 2
                continue
            coeff = token_value(tok)
            if not pos:
                coeff = -coeff
            if not is_minimize:
                coeff = -coeff
            if with_quadratic:
                coeff /= 2.0
            stage = 2
            i += 1
            continue
        if stage == 2:
            if tok == "objconstant":
                offset += coeff
                stage = 0
                i += 1
                continue
            v1 = tok
            variables.add(v1)
            if not with_quadratic:
                linear[v1] += coeff
                stage = 0
                i += 1
                continue
            if i + 1 < len(tokens) and tokens[i + 1] in ("^2", "^", "^ 2"):
                linear[v1] += coeff
                stage = 0
                i += 2
                continue
            if i + 2 >= len(tokens) or tokens[i + 1] != "*":
                raise ValueError(f"cannot parse quadratic near token {i}: {tokens[i:i+5]}")
            v2 = tokens[i + 2]
            variables.add(v2)
            a, b = sorted((v1, v2))
            if a == b:
                linear[a] += coeff
            else:
                quadratic[(a, b)] += coeff
            stage = 0
            i += 3
            continue
    return {
        "sense": sense,
        "linear": dict(linear),
        "quadratic": dict(quadratic),
        "offset": offset,
        "variables": sorted(variables, key=lambda x: (re.sub(r"\d+", "", x), int(re.search(r"\d+", x).group()) if re.search(r"\d+", x) else 0, x)),
    }


def reported_value(internal_energy, sense, offset=0.0):
    if sense == "max":
        return -internal_energy + offset
    return internal_energy + offset


def run_mode16(instance, cutoff):
    cmd = [str(MODE16_BIN), str(cutoff), "1", str(DATA / instance), "1", "16", "3", "8"]
    start = time.time()
    proc = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=cutoff + 30)
    elapsed = time.time() - start
    txt = proc.stdout + "\n" + proc.stderr
    m = re.search(r"best obj (min|max)=\s*([-+0-9.eE]+)", txt)
    return {
        "solver": "mode16",
        "status": "ok" if proc.returncode == 0 else f"exit_{proc.returncode}",
        "reported_obj": float(m.group(2)) if m else None,
        "sense": m.group(1) if m else "",
        "elapsed": elapsed,
        "raw": txt,
    }


def run_tabu(parsed, cutoff):
    import dimod
    from dwave.samplers import TabuSampler
    bqm = dimod.BinaryQuadraticModel(parsed["linear"], parsed["quadratic"], parsed["offset"], dimod.BINARY)
    start = time.time()
    sampleset = TabuSampler().sample(bqm, timeout=int(max(1, cutoff * 1000)), num_reads=1)
    elapsed = time.time() - start
    energy = float(sampleset.first.energy)
    return {
        "solver": "dwave_tabu",
        "status": "ok",
        "reported_obj": reported_value(energy, parsed["sense"], parsed["offset"]),
        "internal_energy": energy,
        "elapsed": elapsed,
    }


def run_neal(parsed, cutoff):
    import dimod
    import neal
    bqm = dimod.BinaryQuadraticModel(parsed["linear"], parsed["quadratic"], parsed["offset"], dimod.BINARY)
    # Neal has sweep/read parameters rather than a hard time limit. Use repeated calls until the
    # cutoff is consumed, keeping the best energy found.
    sampler = neal.SimulatedAnnealingSampler()
    start = time.time()
    best = math.inf
    reads = 0
    while True:
        remaining = cutoff - (time.time() - start)
        if remaining <= 0:
            break
        num_reads = 16
        num_sweeps = 1000
        sampleset = sampler.sample(bqm, num_reads=num_reads, num_sweeps=num_sweeps)
        reads += num_reads
        energy = float(sampleset.first.energy)
        if energy < best:
            best = energy
        if time.time() - start >= cutoff:
            break
    elapsed = time.time() - start
    return {
        "solver": "neal_sa",
        "status": "ok",
        "reported_obj": reported_value(best, parsed["sense"], parsed["offset"]) if best < math.inf else None,
        "internal_energy": best if best < math.inf else None,
        "elapsed": elapsed,
        "reads": reads,
    }


def run_gurobi(parsed, cutoff):
    import gurobipy as gp
    from gurobipy import GRB
    start = time.time()
    model = gp.Model()
    model.Params.OutputFlag = 0
    model.Params.TimeLimit = cutoff
    model.Params.Threads = 1
    vars_by_name = {v: model.addVar(vtype=GRB.BINARY, name=v) for v in parsed["variables"]}
    model.update()
    obj = gp.QuadExpr(parsed["offset"])
    for v, c in parsed["linear"].items():
        obj.add(vars_by_name[v], c)
    for (a, b), c in parsed["quadratic"].items():
        obj.add(vars_by_name[a] * vars_by_name[b], c)
    model.setObjective(obj, GRB.MINIMIZE)
    model.optimize()
    elapsed = time.time() - start
    if model.SolCount == 0:
        val = None
        internal = None
    else:
        internal = float(model.ObjVal)
        val = reported_value(internal, parsed["sense"], parsed["offset"])
    return {
        "solver": "gurobi",
        "status": str(model.Status),
        "reported_obj": val,
        "internal_energy": internal,
        "elapsed": elapsed,
        "mip_gap": float(model.MIPGap) if model.SolCount > 0 and hasattr(model, "MIPGap") else None,
        "bound": reported_value(float(model.ObjBound), parsed["sense"], parsed["offset"]) if model.SolCount > 0 else None,
    }


def compare(a, b, sense):
    if a is None and b is None:
        return "tie"
    if a is None:
        return "loss"
    if b is None:
        return "win"
    if sense == "max":
        if a > b + 1e-6:
            return "win"
        if a < b - 1e-6:
            return "loss"
    else:
        if a < b - 1e-6:
            return "win"
        if a > b + 1e-6:
            return "loss"
    return "tie"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--instances-file")
    ap.add_argument("--instances", nargs="*")
    ap.add_argument("--cutoff", type=int, default=10)
    ap.add_argument("--out-dir", required=True)
    ap.add_argument("--solvers", nargs="+", default=["mode16", "dwave_tabu", "neal_sa", "gurobi"])
    args = ap.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    if args.instances_file:
        instances = [x.strip() for x in Path(args.instances_file).read_text().splitlines() if x.strip()]
    else:
        instances = args.instances or []

    rows = []
    for instance in instances:
        parsed = parse_lp_qubo(DATA / instance)
        mode16 = None
        results = {}
        if "mode16" in args.solvers:
            mode16 = run_mode16(instance, args.cutoff)
            results["mode16"] = mode16
        for solver in args.solvers:
            if solver == "mode16":
                continue
            try:
                if solver == "dwave_tabu":
                    res = run_tabu(parsed, args.cutoff)
                elif solver == "neal_sa":
                    res = run_neal(parsed, args.cutoff)
                elif solver == "gurobi":
                    res = run_gurobi(parsed, args.cutoff)
                else:
                    raise ValueError(f"unknown solver {solver}")
            except Exception as exc:
                res = {"solver": solver, "status": f"error:{type(exc).__name__}:{exc}", "reported_obj": None, "elapsed": 0.0}
            results[solver] = res

        for solver, res in results.items():
            row = {
                "instance": instance,
                "cutoff": args.cutoff,
                "solver": solver,
                "sense": parsed["sense"],
                "nvars": len(parsed["variables"]),
                "nedges": len(parsed["quadratic"]),
                "status": res.get("status", ""),
                "reported_obj": res.get("reported_obj"),
                "elapsed": res.get("elapsed"),
                "mode16_obj": results.get("mode16", {}).get("reported_obj"),
                "solver_vs_mode16": compare(res.get("reported_obj"), results.get("mode16", {}).get("reported_obj"), parsed["sense"]),
                "mode16_vs_solver": compare(results.get("mode16", {}).get("reported_obj"), res.get("reported_obj"), parsed["sense"]),
            }
            for key, value in res.items():
                if key not in row and key != "raw":
                    row[key] = value
            rows.append(row)
            print(instance, solver, row["reported_obj"], row["status"], "elapsed", row["elapsed"], flush=True)

    fields = sorted({k for r in rows for k in r.keys()})
    preferred = ["instance", "cutoff", "solver", "sense", "nvars", "nedges", "status", "reported_obj", "mode16_obj", "mode16_vs_solver", "solver_vs_mode16", "elapsed"]
    fields = preferred + [f for f in fields if f not in preferred]
    with (out_dir / "raw_results.csv").open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)

    summary = []
    for solver in args.solvers:
        if solver == "mode16":
            continue
        sub = [r for r in rows if r["solver"] == solver]
        summary.append({
            "solver": solver,
            "instances": len(sub),
            "mode16_win": sum(1 for r in sub if r["mode16_vs_solver"] == "win"),
            "tie": sum(1 for r in sub if r["mode16_vs_solver"] == "tie"),
            "mode16_loss": sum(1 for r in sub if r["mode16_vs_solver"] == "loss"),
            "solver_errors": sum(1 for r in sub if str(r["status"]).startswith("error")),
        })
    with (out_dir / "summary.csv").open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(summary[0].keys()) if summary else ["solver"])
        writer.writeheader()
        writer.writerows(summary)
    lines = [f"# QUBO Baseline Comparison {args.cutoff}s", ""]
    for s in summary:
        lines.append(f"- Mode16 vs {s['solver']}: {s['mode16_win']}/{s['tie']}/{s['mode16_loss']} (errors={s['solver_errors']})")
    (out_dir / "report.md").write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()
