import os

summary_file = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_test_100/summary.md"
instances_file = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/scripts/phase_f_instances_short.txt"

wins = []
losses = []
balls = []

with open(summary_file, "r") as f:
    for line in f:
        if not line.startswith("|") or "Instance" in line or "---" in line:
            continue
        parts = line.split("|")
        if len(parts) >= 6:
            inst = parts[1].strip()
            old_obj = parts[2].strip()
            new_obj = parts[4].strip()
            
            if "ball" in inst.lower():
                balls.append(inst)
            elif old_obj != "N/A" and new_obj != "N/A":
                try:
                    o = float(old_obj)
                    n = float(new_obj)
                    if n < o:
                        wins.append(inst)
                    elif n > o:
                        losses.append(inst)
                except: pass
            elif old_obj == "N/A" and new_obj != "N/A":
                wins.append(inst)
            elif old_obj != "N/A" and new_obj == "N/A":
                losses.append(inst)

all_selected = set(wins + losses + balls)
with open(instances_file, "w") as f:
    for inst in all_selected:
        f.write(inst + "\n")

print(f"Generated {len(all_selected)} short instances.")
