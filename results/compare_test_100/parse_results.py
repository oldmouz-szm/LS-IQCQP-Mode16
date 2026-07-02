import os

def get_best_obj(log_file):
    best_obj = "N/A"
    time = "N/A"
    try:
        with open(log_file, "r") as f:
            for line in f:
                if "best obj min=" in line:
                    best_obj = line.split("best obj min=")[1].split("wall")[0].strip()
                elif "best obj max=" in line:
                    best_obj = line.split("best obj max=")[1].split("wall")[0].strip()
                if "wall_time_seconds =" in line:
                    time = line.split("wall_time_seconds =")[1].strip()[:6]
    except:
        pass
    return best_obj, time

results_dir = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_test_100"
with open(os.path.join(results_dir, "selected_instances.txt")) as f:
    instances = [line.strip() for line in f if line.strip()]

out_md = open(os.path.join(results_dir, "summary.md"), "w")
out_md.write("| Instance | Old Obj | Old Time (s) | New Obj (full_block) | New Time (s) |\n")
out_md.write("|---|---|---|---|---|\n")

win = 0
tie = 0
loss = 0
valid = 0

for inst in instances:
    old_log = os.path.join(results_dir, f"{inst}_old.log")
    new_log = os.path.join(results_dir, f"{inst}_new.log")
    
    old_obj, old_time = get_best_obj(old_log)
    new_obj, new_time = get_best_obj(new_log)
    
    out_md.write(f"| {inst} | {old_obj} | {old_time} | {new_obj} | {new_time} |\n")
    
    if old_obj != "N/A" and new_obj != "N/A":
        try:
            o_val = float(old_obj)
            n_val = float(new_obj)
            valid += 1
            if n_val < o_val:
                win += 1
            elif n_val > o_val:
                loss += 1
            else:
                tie += 1
        except:
            pass

out_md.write("\n### Summary Statistics\n")
out_md.write(f"- Total Valid Comparisons: {valid}\n")
out_md.write(f"- New Project Wins: {win}\n")
out_md.write(f"- Ties: {tie}\n")
out_md.write(f"- New Project Losses: {loss}\n")
out_md.close()
