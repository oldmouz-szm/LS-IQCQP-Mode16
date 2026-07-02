import os
import glob
import pandas as pd
import numpy as np
from collections import defaultdict

RESULTS_DIR = "/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/results/block_adaptive"

def parse_log(logfile):
    res = {
        'best_obj': np.nan,
        'wall_time': np.nan,
        'block_executed': 0,
        'repair_success': 0,
        'block_time_spent': 0,
        'is_feasible': False,
        'disabled_unconstrained': 0
    }
    try:
        with open(logfile, 'r') as f:
            for line in f:
                if 'best obj min=' in line:
                    try:
                        res['best_obj'] = float(line.split('best obj min=')[1].split('wall')[0].strip())
                        res['is_feasible'] = True
                    except: pass
                elif 'wall_time_seconds =' in line:
                    try: res['wall_time'] = float(line.split('=')[1].strip())
                    except: pass
                elif 'executed_total =' in line:
                    try: res['block_executed'] = int(line.split('=')[1].strip())
                    except: pass
                elif 'repair_success_total =' in line:
                    try: res['repair_success'] = int(line.split('=')[1].strip())
                    except: pass
                elif 'block_time_spent =' in line:
                    try: res['block_time_spent'] = float(line.split('=')[1].strip())
                    except: pass
                elif 'block_disabled_unconstrained_total =' in line:
                    try: res['disabled_unconstrained'] = int(line.split('=')[1].strip())
                    except: pass
                elif 'best obj max=' in line:
                    # just in case it's a maximize problem
                    try:
                        res['best_obj'] = float(line.split('best obj max=')[1].split('wall')[0].strip())
                        res['is_feasible'] = True
                    except: pass
    except Exception as e:
        print(f"Error parsing {logfile}: {e}")
    return res

data = []
logfiles = glob.glob(os.path.join(RESULTS_DIR, "*.log"))

for logfile in logfiles:
    basename = os.path.basename(logfile)
    # Expected format: {inst}.{cutoff}.{name}.seed_{seed}.log
    parts = basename.replace('.log', '').split('.seed_')
    if len(parts) != 2: continue
    seed = int(parts[1])
    prefix = parts[0]
    
    prefix_parts = prefix.split('.')
    inst = prefix_parts[0] + '.lp'
    cutoff = int(prefix_parts[2])
    name = '.'.join(prefix_parts[3:])
    
    parsed = parse_log(logfile)
    parsed['instance'] = inst
    parsed['cutoff'] = cutoff
    parsed['name'] = name
    parsed['seed'] = seed
    data.append(parsed)

df = pd.DataFrame(data)

if len(df) == 0:
    print("No data parsed.")
    exit(0)

# aggregate over seeds
agg_df = df.groupby(['instance', 'cutoff', 'name']).agg(
    avg_obj=('best_obj', 'mean'),
    avg_time=('wall_time', 'mean'),
    avg_block_time=('block_time_spent', 'mean'),
    avg_executed=('block_executed', 'mean'),
    avg_repair_success=('repair_success', 'mean'),
    feasible_count=('is_feasible', 'sum'),
    unconstrained=('disabled_unconstrained', 'mean')
).reset_index()

agg_df.to_csv(os.path.join(RESULTS_DIR, 'aggregate.csv'), index=False)

# Pivot to compare modes for 10s cutoff
df_10s = agg_df[agg_df['cutoff'] == 10]
pivot_10s = df_10s.pivot(index='instance', columns='name', values=['avg_obj', 'avg_executed', 'avg_block_time', 'unconstrained', 'avg_time', 'feasible_count'])

pivot_10s.to_csv(os.path.join(RESULTS_DIR, 'summary.csv'))

# report markdown
md_path = os.path.join(RESULTS_DIR, 'report.md')
with open(md_path, 'w') as f:
    f.write("# Phase F Adaptive Block Trigger Evaluation\n\n")
    
    def compare(m1, m2):
        wins, losses, ties, both_fail, m1_fail, m2_fail = 0, 0, 0, 0, 0, 0
        for inst in pivot_10s.index:
            obj1 = pivot_10s.loc[inst, ('avg_obj', m1)]
            obj2 = pivot_10s.loc[inst, ('avg_obj', m2)]
            f1 = pivot_10s.loc[inst, ('feasible_count', m1)]
            f2 = pivot_10s.loc[inst, ('feasible_count', m2)]
            
            if f1 == 0 and f2 == 0: both_fail += 1
            elif f1 > 0 and f2 == 0: wins += 1; m2_fail += 1
            elif f1 == 0 and f2 > 0: losses += 1; m1_fail += 1
            else:
                if obj1 < obj2 - 1e-4: wins += 1
                elif obj1 > obj2 + 1e-4: losses += 1
                else: ties += 1
        return {'wins': wins, 'losses': losses, 'ties': ties, 'both_fail': both_fail, 'm1_fail': m1_fail, 'm2_fail': m2_fail}

    f.write("## Head-to-Head Comparison (10s Cutoff)\n\n")
    
    comparisons = [
        ('full_block', 'baseline'),
        ('mode_6_adaptive_repair', 'baseline'),
        ('mode_6_adaptive_repair', 'full_block'),
        ('mode_7_adaptive_both', 'full_block'),
        ('mode_6_adaptive_repair', 'mode_7_adaptive_both'),
        ('mode_9_legacy_safe', 'baseline')
    ]
    
    for m1, m2 in comparisons:
        try:
            res = compare(m1, m2)
            f.write(f"- **{m1} vs {m2}**: Wins: {res['wins']}, Losses: {res['losses']}, Ties: {res['ties']} (M1 failed: {res['m1_fail']}, M2 failed: {res['m2_fail']}, Both failed: {res['both_fail']})\n")
        except KeyError:
            f.write(f"- Could not compare {m1} and {m2}\n")

    f.write("\n## Block Overhead & Metrics (10s Cutoff)\n\n")
    f.write("| Mode | Avg Block Executed | Avg Block Time (s) | Avg Repair Success | Avg Unconstrained Disabled |\n")
    f.write("|---|---|---|---|---|\n")
    for name in ['full_block', 'mode_6_adaptive_repair', 'mode_7_adaptive_both', 'mode_9_legacy_safe']:
        try:
            avg_exec = df_10s[df_10s['name'] == name]['avg_executed'].mean()
            avg_time = df_10s[df_10s['name'] == name]['avg_block_time'].mean()
            avg_rep = df_10s[df_10s['name'] == name]['avg_repair_success'].mean()
            avg_unc = df_10s[df_10s['name'] == name]['unconstrained'].mean()
            f.write(f"| {name} | {avg_exec:.2f} | {avg_time:.4f} | {avg_rep:.2f} | {avg_unc:.2f} |\n")
        except: pass

print("Parsing complete. Results in:", RESULTS_DIR)
