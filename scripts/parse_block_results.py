import os
import sys
import glob
import re
import csv
from collections import defaultdict

MODE_NAMES = {
    '0': 'full_block',
    '1': 'repair_only',
    '2': 'improve_only',
    '3': 'smart_only',
    '4': 'normalized_only',
    '5': 'old_block'
}

def parse_log_file(filepath):
    basename = os.path.basename(filepath)
    parts = basename.split('.')
    if len(parts) < 8:
        return None, f"Invalid filename format: {basename}"
        
    seed = parts[-2]
    block_size = parts[-3]
    mode = parts[-4]
    block_flag = parts[-5]
    tabu = parts[-6]
    cutoff = parts[-7]
    instance = ".".join(parts[:-7])
    
    data = {
        'instance': instance,
        'cutoff': cutoff,
        'tabu': tabu,
        'block_flag': block_flag,
        'mode': mode,
        'block_size': block_size,
        'seed': seed,
        'mode_name': 'baseline' if block_flag == '0' else MODE_NAMES.get(mode, 'unknown'),
        'best_obj_min': None,
        'best_obj_max': None,
        'objective_value': None,
        'is_minimize': 1,
        'feasible_found': 0,
        'completed': 0,
        'crashed': 0,
        'timeout_or_no_solution': 0,
        'consistency_error': 0,
        'candidates_total': 0,
        'selected_total': 0,
        'executed_total': 0,
        'repair_success_total': 0,
        'best_update_total': 0,
        'obj_improve_executed_total': 0,
        'violation_improve_executed_total': 0,
        'rejected_by_check_total': 0,
        'wall_time_seconds': 0.0
    }
    
    try:
        with open(filepath, 'r') as f:
            content = f.read()
            
            if "Consistency error" in content or "consistency check failed" in content.lower():
                data['consistency_error'] = 1
                
            if "without equation" in content or "block stats:" in content:
                data['completed'] = 1
            elif "Segmentation fault" in content or "Aborted" in content or "terminate called" in content:
                data['crashed'] = 1
                
            obj_min_match = re.search(r'best obj min=\s*([\d\.\-]+)', content)
            obj_max_match = re.search(r'best obj max=\s*([\d\.\-]+)', content)
            
            if obj_min_match:
                data['best_obj_min'] = float(obj_min_match.group(1))
                data['objective_value'] = data['best_obj_min']
                data['is_minimize'] = 1
                data['feasible_found'] = 1
            elif obj_max_match:
                data['best_obj_max'] = float(obj_max_match.group(1))
                data['objective_value'] = data['best_obj_max']
                data['is_minimize'] = 0
                data['feasible_found'] = 1
            else:
                data['timeout_or_no_solution'] = 1
                
            time_match = re.search(r'wall_time_seconds\s*=\s*([\d\.]+)', content)
            if time_match:
                data['wall_time_seconds'] = float(time_match.group(1))
                
            stats_patterns = {
                'candidates_total': r'candidates_total\s*=\s*(\d+)',
                'selected_total': r'selected_total\s*=\s*(\d+)',
                'executed_total': r'executed_total\s*=\s*(\d+)',
                'repair_success_total': r'repair_success_total\s*=\s*(\d+)',
                'best_update_total': r'best_update_total\s*=\s*(\d+)',
                'obj_improve_executed_total': r'obj_improve_executed_total\s*=\s*(\d+)',
                'violation_improve_executed_total': r'violation_improve_executed_total\s*=\s*(\d+)',
                'rejected_by_check_total': r'rejected_by_check_total\s*=\s*(\d+)'
            }
            
            for key, pattern in stats_patterns.items():
                match = re.search(pattern, content)
                if match:
                    data[key] = int(match.group(1))
                    
    except Exception as e:
        return None, f"Exception reading file: {str(e)}"
        
    return data, None

def win_tie_loss(obj1, obj2, is_minimize):
    if abs(obj1 - obj2) < 1e-6:
        return 'tie'
    if is_minimize:
        return 'win' if obj1 < obj2 else 'loss'
    else:
        return 'win' if obj1 > obj2 else 'loss'

def main():
    if len(sys.argv) < 2:
        print("Usage: python parse_block_results.py <results_dir>")
        sys.exit(1)
        
    results_dir = sys.argv[1]
    log_files = glob.glob(os.path.join(results_dir, "*.log"))
    
    parsed_data = []
    errors = []
    
    for f in log_files:
        data, err = parse_log_file(f)
        if data:
            parsed_data.append((f, data))
        else:
            try:
                with open(f, 'r') as file:
                    content = file.read()
                    snippet = content[:200] if len(content) > 0 else "Empty file"
            except:
                snippet = "Could not read"
            errors.append({
                'filename': os.path.basename(f),
                'error_type': 'ParseError',
                'missing_field': err,
                'snippet': snippet.replace('\n', ' ')
            })
            
    if errors:
        with open(os.path.join(results_dir, "parse_errors.csv"), 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=['filename', 'error_type', 'missing_field', 'snippet'])
            writer.writeheader()
            writer.writerows(errors)
            
    if not parsed_data:
        print("No valid logs parsed.")
        return
        
    # Deduplicate baseline
    unique_data = []
    baseline_seen = set()
    
    for filepath, data in parsed_data:
        if data['block_flag'] == '0':
            key = (data['instance'], data['cutoff'], data['tabu'], data['seed'])
            if key in baseline_seen:
                continue
            baseline_seen.add(key)
        unique_data.append(data)
        
    keys = list(unique_data[0].keys())
    with open(os.path.join(results_dir, "summary.csv"), 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=keys)
        writer.writeheader()
        writer.writerows(unique_data)
        
    # Build pairwise dictionaries
    # key: (instance, cutoff, tabu, seed) -> mode_dict (block_size, mode_name) -> data
    runs_by_key = defaultdict(dict)
    for row in unique_data:
        key = (row['instance'], row['cutoff'], row['tabu'], row['seed'])
        if row['block_flag'] == '0':
            runs_by_key[key]['baseline'] = row
        else:
            m_key = f"{row['mode_name']}_{row['block_size']}"
            runs_by_key[key][m_key] = row
            runs_by_key[key][row['mode_name']] = row # Latest block size for this mode
            
    # Pairwise full block vs baseline
    pairwise_full = []
    for key, modes in runs_by_key.items():
        baseline = modes.get('baseline')
        full_block = modes.get('full_block')
        if not baseline or not full_block:
            continue
            
        instance, cutoff, tabu, seed = key
        b_feas = baseline['feasible_found']
        f_feas = full_block['feasible_found']
        b_obj = baseline['objective_value']
        f_obj = full_block['objective_value']
        
        outcome = 'unsolved_both'
        if b_feas and f_feas:
            outcome = win_tie_loss(f_obj, b_obj, full_block['is_minimize'])
        elif f_feas and not b_feas:
            outcome = 'win'
        elif b_feas and not f_feas:
            outcome = 'loss'
            
        gap = 0
        if b_feas and f_feas and b_obj != 0:
            gap = (f_obj - b_obj) / abs(b_obj)
            
        pairwise_full.append({
            'instance': instance, 'cutoff': cutoff, 'tabu': tabu, 'seed': seed,
            'baseline_feasible': b_feas, 'block_feasible': f_feas,
            'baseline_obj': b_obj, 'block_obj': f_obj,
            'outcome': outcome, 'relative_gap': gap
        })
        
    if pairwise_full:
        with open(os.path.join(results_dir, "pairwise_full_vs_baseline.csv"), 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=list(pairwise_full[0].keys()))
            writer.writeheader()
            writer.writerows(pairwise_full)
            
    # Aggregate data
    groups = defaultdict(list)
    for row in unique_data:
        key = (row['cutoff'], row['tabu'], row['block_flag'], row['mode_name'], row['block_size'])
        groups[key].append(row)
        
    agg_results = []
    for key, rows in groups.items():
        cutoff, tabu, block_flag, mode_name, block_size = key
        
        run_count = len(rows)
        feasible_count = sum(r['feasible_found'] for r in rows)
        
        best_objs = [r['objective_value'] for r in rows if r['objective_value'] is not None]
        avg_best_obj = sum(best_objs) / len(best_objs) if best_objs else None
        
        avg_repair = sum(r['repair_success_total'] for r in rows) / run_count
        avg_obj_imp = sum(r['obj_improve_executed_total'] for r in rows) / run_count
        avg_viol_imp = sum(r['violation_improve_executed_total'] for r in rows) / run_count
        avg_exec = sum(r['executed_total'] for r in rows) / run_count
        avg_cand = sum(r['candidates_total'] for r in rows) / run_count
        avg_time = sum(r['wall_time_seconds'] for r in rows) / run_count if any(r['wall_time_seconds'] > 0 for r in rows) else 0
        
        agg_results.append({
            'cutoff': cutoff, 'tabu': tabu, 'block_flag': block_flag, 'mode_name': mode_name, 'block_size': block_size,
            'run_count': run_count, 'feasible_count': feasible_count,
            'avg_best_obj': avg_best_obj,
            'avg_repair_success_total': avg_repair,
            'avg_obj_improve_executed_total': avg_obj_imp,
            'avg_violation_improve_executed_total': avg_viol_imp,
            'avg_executed_total': avg_exec,
            'avg_candidates_total': avg_cand,
            'avg_wall_time': avg_time
        })
        
    if agg_results:
        with open(os.path.join(results_dir, "aggregate.csv"), 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=list(agg_results[0].keys()))
            writer.writeheader()
            writer.writerows(agg_results)
            
    # Pairwise modes
    comparisons = [
        ('full_block', 'baseline'),
        ('repair_only', 'baseline'),
        ('improve_only', 'baseline'),
        ('smart_only', 'old_block'),
        ('normalized_only', 'old_block'),
        ('full_block', 'repair_only')
    ]
    
    pairwise_modes = []
    for mode_a, mode_b in comparisons:
        win = tie = loss = unsolved_both = 0
        gap_sum = 0
        gap_count = 0
        feas_a = feas_b = 0
        
        for key, modes in runs_by_key.items():
            if mode_a in modes and mode_b in modes:
                ma = modes[mode_a]
                mb = modes[mode_b]
                fa, fb = ma['feasible_found'], mb['feasible_found']
                feas_a += fa
                feas_b += fb
                
                if fa and fb:
                    outcome = win_tie_loss(ma['objective_value'], mb['objective_value'], ma['is_minimize'])
                    if outcome == 'win': win += 1
                    elif outcome == 'loss': loss += 1
                    else: tie += 1
                    if mb['objective_value'] != 0:
                        gap_sum += (ma['objective_value'] - mb['objective_value']) / abs(mb['objective_value'])
                        gap_count += 1
                elif fa and not fb:
                    win += 1
                elif fb and not fa:
                    loss += 1
                else:
                    unsolved_both += 1
                    
        if win + tie + loss + unsolved_both > 0:
            pairwise_modes.append({
                'comparison': f"{mode_a} vs {mode_b}",
                'win': win, 'tie': tie, 'loss': loss, 'unsolved_both': unsolved_both,
                'avg_relative_gap': gap_sum / gap_count if gap_count > 0 else 0,
                'feasible_gain': feas_a - feas_b
            })
            
    if pairwise_modes:
        with open(os.path.join(results_dir, "pairwise_modes.csv"), 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=list(pairwise_modes[0].keys()))
            writer.writeheader()
            writer.writerows(pairwise_modes)
            
    # Tables for report
    tables_dir = os.path.join(results_dir, "tables")
    os.makedirs(tables_dir, exist_ok=True)
    
    mode_feas = defaultdict(lambda: [0, 0]) # feas, total
    for row in unique_data:
        mode_feas[row['mode_name']][0] += row['feasible_found']
        mode_feas[row['mode_name']][1] += 1
        
    with open(os.path.join(tables_dir, "table_feasibility.csv"), 'w') as f:
        f.write("mode,feasible_count,total_runs,feasible_rate\n")
        for mode, (feas, total) in mode_feas.items():
            f.write(f"{mode},{feas},{total},{feas/total if total>0 else 0:.4f}\n")
            
    if pairwise_modes:
        with open(os.path.join(tables_dir, "table_win_loss.csv"), 'w') as f:
            f.write("comparison,win,tie,loss,unsolved_both\n")
            for row in pairwise_modes:
                f.write(f"{row['comparison']},{row['win']},{row['tie']},{row['loss']},{row['unsolved_both']}\n")
                
    bs_stats = defaultdict(lambda: [0, 0, 0.0, 0.0, 0.0]) # run, feas, obj, time, exec
    for row in unique_data:
        if row['block_flag'] == '1':
            bs = row['block_size']
            bs_stats[bs][0] += 1
            bs_stats[bs][1] += row['feasible_found']
            if row['objective_value'] is not None:
                bs_stats[bs][2] += row['objective_value']
            bs_stats[bs][3] += row['wall_time_seconds']
            bs_stats[bs][4] += row['executed_total']
            
    with open(os.path.join(tables_dir, "table_block_size.csv"), 'w') as f:
        f.write("block_size,feasible_rate,avg_best_obj,avg_wall_time,avg_executed_total\n")
        for bs, stats in bs_stats.items():
            if stats[0] > 0:
                f.write(f"{bs},{stats[1]/stats[0]:.4f},{stats[2]/stats[1] if stats[1]>0 else 0:.4f},{stats[3]/stats[0]:.4f},{stats[4]/stats[0]:.4f}\n")
                
    with open(os.path.join(tables_dir, "table_ablation_modes.csv"), 'w') as f:
        f.write("mode,feasible_rate,avg_best_obj,avg_repair_success_total,avg_obj_improve_executed_total,avg_violation_improve_executed_total\n")
        for row in agg_results:
            if row['block_size'] == '3' or row['mode_name'] == 'baseline':
                fr = row['feasible_count'] / row['run_count'] if row['run_count'] > 0 else 0
                obj = row['avg_best_obj'] if row['avg_best_obj'] is not None else 0
                f.write(f"{row['mode_name']},{fr:.4f},{obj:.4f},{row['avg_repair_success_total']:.4f},{row['avg_obj_improve_executed_total']:.4f},{row['avg_violation_improve_executed_total']:.4f}\n")

    # Generate report.md
    report_path = os.path.join(results_dir, "report.md")
    with open(report_path, 'w') as f:
        f.write("# Block Move Ablation Report\n\n")
        f.write("## 1. Setup\n")
        f.write("- **Project Path**: `/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP`\n")
        f.write("- **Instances**: `/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/data/all_lp`\n\n")
        
        f.write("## 2. Run Status\n")
        f.write(f"- Completed Runs: {len(unique_data)}\n")
        f.write(f"- Parse Errors: {len(errors)}\n")
        f.write(f"- Consistency Errors: {sum(r['consistency_error'] for r in unique_data)}\n\n")
        
        f.write("## 3. Feasibility Results\n")
        f.write("Mode | Feasible | Total | Rate\n---|---|---|---\n")
        for mode, (feas, total) in mode_feas.items():
            f.write(f"{mode} | {feas} | {total} | {feas/total if total>0 else 0:.2%}\n")
        
        f.write("\n## 4. Objective Results\n")
        f.write("Comparison | Win | Tie | Loss | Unsolved\n---|---|---|---|---\n")
        for row in pairwise_modes:
            f.write(f"{row['comparison']} | {row['win']} | {row['tie']} | {row['loss']} | {row['unsolved_both']}\n")
            
        f.write("\n## 5. Ablation Study\n")
        f.write("- **Smart vs Old**: Review `table_win_loss.csv` for smart_only vs old_block.\n")
        f.write("- **Repair vs Full**: Review feasibility vs best objective trade-offs.\n")
        f.write("- **Block Size**: Check `table_block_size.csv` for the optimal size.\n\n")
        
        f.write("## 6. Cost Analysis\n")
        f.write("- Check `table_block_size.csv` for avg_wall_time impact.\n\n")
        
        f.write("## 7. Findings\n")
        f.write("*(Auto-generated placeholder - Block move's main contribution is often feasibility repair. Verify if objective stabilizes.)*\n\n")
        f.write("## 8. Next Steps\n")
        f.write("- Review figures in `figures/` directory.\n")
        f.write("- Note: 当前结论只基于项目样例目录 data/all_lp 的测试，不能直接代表完整 QPLIB/MINLPLIB。\n")

if __name__ == "__main__":
    main()
