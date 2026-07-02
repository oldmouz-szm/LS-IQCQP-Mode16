import os
import sys

def get_stats(log_file):
    best_obj = "N/A"
    time = "N/A"
    solution_status = None
    try:
        with open(log_file, "r") as f:
            for line in f:
                if "best obj min=" in line:
                    best_obj = line.split("best obj min=")[1].split("wall")[0].strip()
                elif "best obj max=" in line:
                    best_obj = line.split("best obj max=")[1].split("wall")[0].strip()
                elif "solution_status =" in line:
                    solution_status = line.split("solution_status =", 1)[1].strip()
                if "wall_time_seconds =" in line:
                    time = line.split("wall_time_seconds =")[1].strip()[:6]
    except:
        pass
    if solution_status is not None and solution_status != "FEASIBLE":
        best_obj = "N/A"
    return best_obj, time

if len(sys.argv) < 2:
    print("Usage: python3 parse_all_serial.py <results_dir>")
    sys.exit(1)

results_dir = sys.argv[1]

# Get instances from log files
instances = []
for file in os.listdir(results_dir):
    if file.endswith("_baseline.log"):
        instances.append(file.replace("_baseline.log", ""))

# Sort for deterministic output
instances.sort()

out_md = open(os.path.join(results_dir, "report.md"), "w")
out_md.write("# Serial 221 Instances Comparison (Mode 12 vs Baseline)\n\n")

out_md.write("## 1. Instance Level Best Objs\n\n")
out_md.write("| Instance | Baseline | Baseline Time | Mode 12 | Mode 12 Time |\n")
out_md.write("|---|---|---|---|---|\n")

wins = 0
ties = 0
losses = 0
valid = 0

for inst in instances:
    base_log = os.path.join(results_dir, f"{inst}_baseline.log")
    mode_log = os.path.join(results_dir, f"{inst}_mode12.log")
    
    base_obj, base_time = get_stats(base_log)
    mode_obj, mode_time = get_stats(mode_log)
    
    row = [inst, base_obj, base_time, mode_obj, mode_time]
    out_md.write("| " + " | ".join(row) + " |\n")
    
    if base_obj != "N/A" and mode_obj != "N/A":
        valid += 1
        if float(mode_obj) < float(base_obj):
            wins += 1
        elif float(mode_obj) > float(base_obj):
            losses += 1
        else:
            ties += 1

out_md.write("\n## 2. Summary vs Baseline\n\n")
out_md.write("| Valid | Wins | Ties | Losses |\n")
out_md.write("|---|---|---|---|\n")
out_md.write(f"| {valid} | {wins} | {ties} | {losses} |\n")

out_md.close()
print("Parsing completed.")
