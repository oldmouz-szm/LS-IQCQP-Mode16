import os
import sys
import glob

def check_status(results_dir):
    log_files = glob.glob(os.path.join(results_dir, "*.log"))
    
    completed_runs = 0
    failed_runs = 0
    consistency_errors = 0
    no_obj = 0
    no_stats = 0
    
    missing_runs = []
    expected_runs = 0
    # To determine missing, we would ideally read the instances but we just write a placeholder
    # or infer from missing seeds for completed instances.
    
    for f in log_files:
        basename = os.path.basename(f)
        try:
            with open(f, 'r') as file:
                content = file.read()
                if "block stats:" in content or "without equation" in content:
                    completed_runs += 1
                else:
                    failed_runs += 1
                
                if "Consistency error" in content or "consistency check failed" in content.lower():
                    consistency_errors += 1
                    
                if "best obj min" not in content and "best obj max" not in content:
                    no_obj += 1
                    
                if "block stats:" not in content and ".baseline." not in basename:
                    no_stats += 1
        except Exception as e:
            failed_runs += 1
            
    # Simple expected runs calculation is complex without scanning the scripts config.
    # We will let the user know what was found.
    print("=== Block Ablation Status ===")
    print(f"Total log files: {len(log_files)}")
    print(f"Completed runs: {completed_runs}")
    print(f"Failed runs: {failed_runs}")
    print(f"Logs with consistency error: {consistency_errors}")
    print(f"Logs without best obj: {no_obj}")
    print(f"Logs without block stats (excluding baseline): {no_stats}")
    
    with open(os.path.join(results_dir, "missing_runs.txt"), 'w') as f:
        f.write("Expected runs: unknown (needs full scan)\n")
        f.write("Missing runs: check log counts compared to instance count.\n")
    
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python check_block_ablation_status.py <results_dir>")
        sys.exit(1)
    check_status(sys.argv[1])
