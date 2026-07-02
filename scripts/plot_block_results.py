import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_block_results.py <results_dir>")
        sys.exit(1)
        
    results_dir = sys.argv[1]
    figures_dir = os.path.join(results_dir, "figures")
    os.makedirs(figures_dir, exist_ok=True)
    
    summary_csv = os.path.join(results_dir, "summary.csv")
    pairwise_modes_csv = os.path.join(results_dir, "pairwise_modes.csv")
    
    if not os.path.exists(summary_csv):
        print("No summary.csv found.")
        return
        
    df = pd.read_csv(summary_csv)
    
    # Set style
    sns.set_theme(style="whitegrid")
    
    def save_plot(name):
        plt.tight_layout()
        plt.savefig(os.path.join(figures_dir, f"{name}.png"), dpi=300)
        plt.savefig(os.path.join(figures_dir, f"{name}.pdf"))
        plt.close()
        
    # 1. Feasible rate by mode
    plt.figure(figsize=(10, 6))
    feas_rate = df.groupby('mode_name')['feasible_found'].mean().reset_index()
    sns.barplot(data=feas_rate, x='mode_name', y='feasible_found', palette="viridis")
    plt.title('Feasible Rate by Mode')
    plt.ylabel('Feasible Rate')
    plt.xticks(rotation=45)
    save_plot('feasible_rate_by_mode')
    
    # 2. Average best objective by mode
    plt.figure(figsize=(10, 6))
    df_feas = df[df['feasible_found'] == 1]
    if not df_feas.empty:
        sns.boxplot(data=df_feas, x='mode_name', y='objective_value', palette="viridis")
        plt.title('Objective Value Distribution by Mode (Feasible only)')
        plt.ylabel('Objective Value')
        plt.xticks(rotation=45)
        save_plot('objective_by_mode')
        
    # 3. Block size vs runtime
    plt.figure(figsize=(8, 6))
    df_block = df[df['block_flag'] == 1]
    if not df_block.empty:
        sns.boxplot(data=df_block, x='block_size', y='wall_time_seconds', palette="Set2")
        plt.title('Wall-clock Time by Block Size')
        plt.ylabel('Time (s)')
        save_plot('runtime_by_block_size')
        
    # 4. Block size vs objective
    plt.figure(figsize=(8, 6))
    df_block_feas = df_block[df_block['feasible_found'] == 1]
    if not df_block_feas.empty:
        sns.boxplot(data=df_block_feas, x='block_size', y='objective_value', palette="Set2")
        plt.title('Objective Value by Block Size')
        plt.ylabel('Objective Value')
        save_plot('objective_by_block_size')

    # Pairwise plots if available
    if os.path.exists(pairwise_modes_csv):
        pm_df = pd.read_csv(pairwise_modes_csv)
        if not pm_df.empty:
            plt.figure(figsize=(12, 6))
            pm_melt = pm_df.melt(id_vars='comparison', value_vars=['win', 'tie', 'loss'], 
                                 var_name='outcome', value_name='count')
            sns.barplot(data=pm_melt, x='comparison', y='count', hue='outcome', palette={'win': 'g', 'tie': 'gray', 'loss': 'r'})
            plt.title('Win/Tie/Loss for Mode Comparisons')
            plt.xticks(rotation=45)
            save_plot('win_tie_loss_comparisons')
            
    print("Plots generated successfully.")

if __name__ == "__main__":
    main()
