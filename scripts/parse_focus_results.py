import os
import sys

def get_stats(log_file):
    best_obj = "N/A"
    time = "N/A"
    block_time = 0.0
    block_executed = 0
    repair_success = 0
    try:
        with open(log_file, "r") as f:
            for line in f:
                if "best obj min=" in line:
                    best_obj = line.split("best obj min=")[1].split("wall")[0].strip()
                elif "best obj max=" in line:
                    best_obj = line.split("best obj max=")[1].split("wall")[0].strip()
                if "wall_time_seconds =" in line:
                    time = line.split("wall_time_seconds =")[1].strip()[:6]
                if "block_time_spent =" in line:
                    block_time = float(line.split("=")[1].strip())
                if "executed_total =" in line:
                    block_executed = int(line.split("=")[1].strip())
                if "block_violation_improve_executed_total =" in line:
                    repair_success += int(line.split("=")[1].strip())
                if "block_keep_feasible_executed_total =" in line:
                    repair_success += int(line.split("=")[1].strip())
    except:
        pass
    return best_obj, time, block_time, block_executed, repair_success

if len(sys.argv) < 2:
    print("Usage: python3 parse_focus_results.py <results_dir>")
    sys.exit(1)

results_dir = sys.argv[1]
with open("scripts/focus_instances.txt") as f:
    instances = [line.strip() for line in f if line.strip()]

modes = ["baseline", "6", "10", "11", "12"]
seeds = ["8", "9", "10"]

# summary file
out_md = open(os.path.join(results_dir, "report.md"), "w")
out_md.write("# Phase F-2 Focus Test Report\n\n")

out_md.write("## 1. Instance Level Best Objs\n\n")
out_md.write("| Instance | Baseline | Mode 6 | Mode 10 | Mode 11 | Mode 12 |\n")
out_md.write("|---|---|---|---|---|---|\n")

mode_wins = {m: 0 for m in modes if m != "baseline"}
mode_ties = {m: 0 for m in modes if m != "baseline"}
mode_losses = {m: 0 for m in modes if m != "baseline"}
mode_valid = {m: 0 for m in modes if m != "baseline"}

mode_stats = {m: {"time": 0.0, "exec": 0, "repair_succ": 0} for m in modes if m != "baseline"}

for inst in instances:
    # get baseline
    base_log = os.path.join(results_dir, f"{inst}.cutoff10.mode_baseline.seed8.log")
    base_obj, _, _, _, _ = get_stats(base_log)
    
    row = [inst, base_obj]
    
    for mode in ["6", "10", "11", "12"]:
        best_val = None
        sum_time = 0.0
        sum_exec = 0
        sum_succ = 0
        for seed in seeds:
            log = os.path.join(results_dir, f"{inst}.cutoff10.mode{mode}.seed{seed}.log")
            obj, _, t, exe, succ = get_stats(log)
            sum_time += t
            sum_exec += exe
            sum_succ += succ
            if obj != "N/A":
                val = float(obj)
                if best_val is None or val < best_val:
                    best_val = val
        
        mode_stats[mode]["time"] += sum_time / 3.0
        mode_stats[mode]["exec"] += sum_exec / 3.0
        mode_stats[mode]["repair_succ"] += sum_succ / 3.0
        
        mode_obj = f"{best_val}" if best_val is not None else "N/A"
        row.append(mode_obj)
        
        if base_obj != "N/A" and mode_obj != "N/A":
            mode_valid[mode] += 1
            if float(mode_obj) < float(base_obj):
                mode_wins[mode] += 1
            elif float(mode_obj) > float(base_obj):
                mode_losses[mode] += 1
            else:
                mode_ties[mode] += 1

    out_md.write("| " + " | ".join(row) + " |\n")

out_md.write("\n## 2. Summary vs Baseline\n\n")
out_md.write("| Mode | Valid | Wins | Ties | Losses | Avg Block Time (s) | Avg Block Execs | Avg Repair Success |\n")
out_md.write("|---|---|---|---|---|---|---|---|\n")
for mode in ["6", "10", "11", "12"]:
    avg_time = mode_stats[mode]["time"] / len(instances)
    avg_exec = mode_stats[mode]["exec"] / len(instances)
    avg_succ = mode_stats[mode]["repair_succ"] / len(instances)
    out_md.write(f"| {mode} | {mode_valid[mode]} | {mode_wins[mode]} | {mode_ties[mode]} | {mode_losses[mode]} | {avg_time:.4f} | {avg_exec:.1f} | {avg_succ:.1f} |\n")

out_md.close()
