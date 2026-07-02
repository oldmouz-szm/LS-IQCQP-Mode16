import os

def get_best_obj(log_file):
    best_obj = "N/A"
    time = "N/A"
    try:
        with open(log_file, "r") as f:
            for line in f:
                if "best obj min=" in line or "best obj max=" in line:
                    best_obj = line.split("=")[-1].strip()
                elif "wall_time_seconds =" in line:
                    time = line.split("=")[-1].strip()[:6]
    except:
        pass
    return best_obj, time

print("| Instance | Old Project (szm/LS-IQCQP) | Time | New Project (IQCQP-block) | Time |")
print("|---|---|---|---|---|")
instances = ["QPLIB_3347.lp", "QPLIB_2880.lp", "QPLIB_6597.lp", "pb302035.lp", "pb302055.lp", "pb302075.lp"]
results_dir = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_test"

for inst in instances:
    old_log = os.path.join(results_dir, f"{inst}_old.log")
    new_log = os.path.join(results_dir, f"{inst}_new.log")
    
    old_obj, old_time = get_best_obj(old_log)
    new_obj, new_time = get_best_obj(new_log)
    
    print(f"| {inst} | {old_obj} | {old_time}s | {new_obj} | {new_time}s |")
