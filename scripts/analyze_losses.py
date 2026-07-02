import re
import os

results_dir = "results/compare_test_100"
data_dir = "data/all_lp"
summary_file = os.path.join(results_dir, "summary.md")

losses = []

with open(summary_file, 'r') as f:
    for line in f:
        if line.startswith('|') and 'Old Obj' not in line and '---' not in line:
            parts = [p.strip() for p in line.split('|')]
            if len(parts) >= 6:
                inst = parts[1]
                old_obj = parts[2]
                new_obj = parts[4]
                if old_obj != 'N/A' and new_obj != 'N/A':
                    try:
                        o_val = float(old_obj)
                        n_val = float(new_obj)
                        if n_val > o_val:
                            losses.append(inst)
                    except ValueError:
                        pass

def parse_lp_header(lp_file):
    stats = {'vars': 'N/A', 'cons': 'N/A', 'NL': 'N/A'}
    if not os.path.exists(lp_file):
        return stats
    with open(lp_file, 'r') as f:
        lines = []
        for _ in range(30):
            lines.append(f.readline())
        content = "".join(lines)
        
        # Parse Equation counts
        eq_match = re.search(r'\\ Equation counts\s*\\.*?\s*\\\s+(\d+)', content, re.MULTILINE)
        if eq_match: stats['cons'] = int(eq_match.group(1))
        
        # Parse Variable counts
        var_match = re.search(r'\\ Variable counts\s*\\.*?\s*\\\s+(\d+)', content, re.MULTILINE)
        if var_match: stats['vars'] = int(var_match.group(1))
        
        # Parse Nonzero counts
        nz_match = re.search(r'\\ Nonzero counts\s*\\.*?\s*\\\s+\d+\s+\d+\s+(\d+)', content, re.MULTILINE)
        if nz_match: stats['NL'] = int(nz_match.group(1))
        
    return stats

def parse_steps(log_file):
    if not os.path.exists(log_file): return 'N/A'
    with open(log_file, 'r') as f:
        content = f.read()
        match = re.search(r'\[final\] step (\d+)', content)
        if match: return int(match.group(1))
    return 'N/A'

print(f"{'Instance':<32} {'Vars':<10} {'Cons':<10} {'QuadTerms':<10} {'Steps(Old)':<12} {'Steps(New)':<12}")
for inst in losses:
    lp_path = os.path.join(data_dir, inst)
    lp_stats = parse_lp_header(lp_path)
    
    old_log = os.path.join(results_dir, f"{inst}_old.log")
    new_log = os.path.join(results_dir, f"{inst}_new.log")
    
    steps_o = parse_steps(old_log)
    steps_n = parse_steps(new_log)
    
    print(f"{inst:<32} {lp_stats['vars']:<10} {lp_stats['cons']:<10} {lp_stats['NL']:<10} {steps_o:<12} {steps_n:<12}")
