import os
import random

summary_file = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/compare_test_100/summary.md"
instances_file = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/scripts/phase_f_instances.txt"

wins = []
losses = []
ties = []
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
            
            if old_obj != "N/A" and new_obj != "N/A":
                try:
                    o = float(old_obj)
                    n = float(new_obj)
                    if n < o:
                        wins.append(inst)
                    elif n > o:
                        losses.append(inst)
                    else:
                        ties.append(inst)
                except ValueError:
                    pass
            elif old_obj == "N/A" and new_obj != "N/A":
                wins.append(inst) # old failed, new found feasible
            elif old_obj != "N/A" and new_obj == "N/A":
                losses.append(inst) # old found feasible, new failed

# Get ball instances from tie/win/loss or from list
for inst in wins + losses + ties:
    if "ball" in inst.lower():
        balls.append(inst)

# Remove balls from wins/losses/ties to avoid duplicates
wins = [i for i in wins if i not in balls]
losses = [i for i in losses if i not in balls]
ties = [i for i in ties if i not in balls]

random.seed(42)
sample_ties = random.sample(ties, min(20, len(ties)))

large_instances = ["QPLIB_3347.lp", "QPLIB_2880.lp", "QPLIB_6597.lp", "pb302035.lp", "pb302055.lp", "pb302075.lp"]

all_selected = set(wins + losses + sample_ties + balls + large_instances)

with open(instances_file, "w") as f:
    for inst in all_selected:
        f.write(inst + "\n")

print(f"Generated {len(all_selected)} instances for Phase F testing.")
print(f"Wins: {len(wins)}, Losses: {len(losses)}, Ties: {len(sample_ties)}, Balls: {len(balls)}, Large: {len(large_instances)}")
