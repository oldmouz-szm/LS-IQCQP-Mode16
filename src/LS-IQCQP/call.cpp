#include <stdio.h>
#include "sol.h"
int main(int argc,char *argv[]) // pri
{
    int flag = 2;
    if (flag == 0)
    {
        solver::qp_solver * ls_qp_solver = new solver::qp_solver;
        //读入文件和时间参数
        ls_qp_solver->read(argv[2]);
        ls_qp_solver->_cut_off = std::atof(argv[1]);
        ls_qp_solver->print_lp_formula(true);
        // ls_qp_solver->print_lp_formula(true);
        exit(0);//第一次
    }
    else if (flag == 1)
    {
        solver::qp_solver * ls_qp_solver = new solver::qp_solver;
        //读入文件和时间参数
        ls_qp_solver->read(argv[2]);
        ls_qp_solver->_cut_off = std::atof(argv[1]);
        // ls_qp_solver->seed_num = std::atoi(argv[3]);
        //读入初始解
        ls_qp_solver->read_init_solution(argv[3]); // 接收lp全文格式的初始解
        // exit(0);
        ls_qp_solver->local_search(); ls_qp_solver->debug_check_solution_consistency("final"); ls_qp_solver->print_formula(); ls_qp_solver->print_best_solution();
        // // cout << endl;
        // freopen(argv[5], "w", stdout);
        // ls_qp_solver->print_formula(); ls_qp_solver->print_best_solution();
        // fclose(stdout);
        // ls_qp_solver->print_formula(); ls_qp_solver->print_best_solution();
    }
    else if (flag == 2)
    {
        solver::qp_solver * ls_qp_solver = new solver::qp_solver;
        ls_qp_solver->read(argv[3]);
        ls_qp_solver->_cut_off = std::atof(argv[1]);
        ls_qp_solver->tabu_switch = std::atoi(argv[2]);
        if (ls_qp_solver->tabu_switch == 1)
        {
            ls_qp_solver->set_constraint_tabu_enabled(true);
        }
        
        ls_qp_solver->seed_num = 8;
        if (argc >= 8) {
            ls_qp_solver->seed_num = std::atoi(argv[7]);
        }
        std::srand(ls_qp_solver->seed_num);

        if (argc >= 5) {
            ls_qp_solver->block_move_enabled = std::atoi(argv[4]) != 0;
        }
        
        if (argc >= 6 && ls_qp_solver->block_move_enabled) {
            int mode = std::atoi(argv[5]);
            if (mode == 0) {
                // full block (pre-Phase F)
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 1) {
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 2) {
                ls_qp_solver->block_repair_enabled = false;
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 3) {
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 4) {
                ls_qp_solver->block_smart_candidate_enabled = false;
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 5) {
                ls_qp_solver->block_smart_candidate_enabled = false;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 6) {
                // adaptive repair only
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = true;
            } else if (mode == 7) {
                // adaptive repair + adaptive improve
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_improve_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = true;
            } else if (mode == 8) {
                // adaptive repair + original score
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
            } else if (mode == 9) {
                // legacy-safe mode (block off or minimal interference)
                ls_qp_solver->block_move_enabled = false;
                ls_qp_solver->block_adaptive_trigger_enabled = false;
            } else if (mode == 10) {
                // conservative adaptive repair
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 500;
                ls_qp_solver->block_min_steps_before_enable = 500;
                ls_qp_solver->block_trigger_interval = 100;
                ls_qp_solver->block_time_budget_ratio = 0.05;
            } else if (mode == 11) {
                // ultra-conservative adaptive repair
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 1000;
                ls_qp_solver->block_min_steps_before_enable = 1000;
                ls_qp_solver->block_trigger_interval = 200;
                ls_qp_solver->block_time_budget_ratio = 0.03;
            } else if (mode == 12) {
                // feasible-lock adaptive repair
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 500;
                ls_qp_solver->block_min_steps_before_enable = 500;
                ls_qp_solver->block_trigger_interval = 100;
                ls_qp_solver->block_time_budget_ratio = 0.05;
                ls_qp_solver->block_disable_repair_after_first_feasible = true;
            } else if (mode == 13) {
                // violation-prioritized safe feasible-lock adaptive repair
                // Phase G: Violation-Prioritized Safe Block Repair
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 500;
                ls_qp_solver->block_min_steps_before_enable = 500;
                ls_qp_solver->block_trigger_interval = 100;
                ls_qp_solver->block_time_budget_ratio = 0.05;
                ls_qp_solver->block_disable_repair_after_first_feasible = true;
                ls_qp_solver->block_prioritized_repair_enabled = true;
                ls_qp_solver->block_safe_acceptance_enabled = true;
                ls_qp_solver->block_top_cons_num = 5;
                ls_qp_solver->block_safe_min_relative_gain = 1e-9;
            } else if (mode == 14) {
                // Phase H: Correctness-Preserving Serial Hybrid
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 500;
                ls_qp_solver->block_min_steps_before_enable = 500;
                ls_qp_solver->block_trigger_interval = 100;
                ls_qp_solver->block_time_budget_ratio = 0.05;
                ls_qp_solver->block_disable_repair_after_first_feasible = true;
                ls_qp_solver->block_prioritized_repair_enabled = true;
                ls_qp_solver->block_safe_acceptance_enabled = true;
                ls_qp_solver->block_top_cons_num = 5;
                ls_qp_solver->block_safe_min_relative_gain = 1e-9;
                ls_qp_solver->legacy_seed_schedule_enabled = true;
                ls_qp_solver->strict_incumbent_validation_enabled = false;
            } else if (mode == 15) {
                // Phase H-2: serial iterated-local-search hybrid
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 500;
                ls_qp_solver->block_min_steps_before_enable = 500;
                ls_qp_solver->block_trigger_interval = 100;
                ls_qp_solver->block_time_budget_ratio = 0.05;
                ls_qp_solver->block_disable_repair_after_first_feasible = true;
                ls_qp_solver->block_prioritized_repair_enabled = true;
                ls_qp_solver->block_safe_acceptance_enabled = true;
                ls_qp_solver->block_top_cons_num = 5;
                ls_qp_solver->block_safe_min_relative_gain = 1e-9;
                ls_qp_solver->legacy_seed_schedule_enabled = true;
                ls_qp_solver->strict_incumbent_validation_enabled = false;
                ls_qp_solver->serial_diversification_enabled = true;
                ls_qp_solver->broad_escape_pool_enabled = true;
                ls_qp_solver->serial_diversification_time_ratio = 0.85;
                ls_qp_solver->serial_diversification_kick_cap = 8;
                ls_qp_solver->qubo_incremental_gain_enabled = true;
            } else if (mode == 16) {
                // Phase H-3: Mode 15 + QUBO adaptive 2-flip escape
                ls_qp_solver->block_adaptive_trigger_enabled = true;
                ls_qp_solver->block_repair_enabled = true;
                ls_qp_solver->block_improve_enabled = false;
                ls_qp_solver->block_smart_candidate_enabled = true;
                ls_qp_solver->block_normalized_score_enabled = false;
                ls_qp_solver->block_repair_stagnation_window = 500;
                ls_qp_solver->block_min_steps_before_enable = 500;
                ls_qp_solver->block_trigger_interval = 100;
                ls_qp_solver->block_time_budget_ratio = 0.05;
                ls_qp_solver->block_disable_repair_after_first_feasible = true;
                ls_qp_solver->block_prioritized_repair_enabled = true;
                ls_qp_solver->block_safe_acceptance_enabled = true;
                ls_qp_solver->block_top_cons_num = 5;
                ls_qp_solver->block_safe_min_relative_gain = 1e-9;
                ls_qp_solver->legacy_seed_schedule_enabled = true;
                ls_qp_solver->strict_incumbent_validation_enabled = false;
                ls_qp_solver->serial_diversification_enabled = true;
                ls_qp_solver->broad_escape_pool_enabled = true;
                ls_qp_solver->serial_diversification_time_ratio = 0.85;
                ls_qp_solver->serial_diversification_kick_cap = 8;
                ls_qp_solver->qubo_incremental_gain_enabled = true;
                ls_qp_solver->qubo_pair_flip_enabled = true;
            }
        }
        
        if (argc >= 7 && ls_qp_solver->block_move_enabled) {
            ls_qp_solver->block_max_size = std::atoi(argv[6]);
        }

        if (ls_qp_solver->legacy_seed_schedule_enabled) {
            if (ls_qp_solver->_constraints.empty()) {
                ls_qp_solver->seed_num = 4;
            } else if ((ls_qp_solver->_vars.size() > ls_qp_solver->_int_vars.size()
                        && !ls_qp_solver->_int_vars.empty())
                       || ls_qp_solver->_int_vars.size() > ls_qp_solver->_bool_vars.size()) {
                ls_qp_solver->seed_num = 1;
            } else {
                ls_qp_solver->seed_num = 8;
            }
            std::srand(ls_qp_solver->seed_num);
        }

        if (argc >= 9 && ls_qp_solver->serial_diversification_enabled) {
            Float ratio = std::atof(argv[8]);
            if (ratio > 0 && ratio < 1) {
                ls_qp_solver->serial_diversification_time_ratio = ratio;
            }
        }
        if (argc >= 10 && ls_qp_solver->serial_diversification_enabled) {
            int kick_cap = std::atoi(argv[9]);
            if (kick_cap >= 2) {
                ls_qp_solver->serial_diversification_kick_cap = kick_cap;
            }
        }
        if (argc >= 11 && ls_qp_solver->qubo_incremental_gain_enabled) {
            long long verify_interval = std::atoll(argv[10]);
            if (verify_interval >= 0) {
                ls_qp_solver->qubo_gain_verify_interval = verify_interval;
            }
        }

        ls_qp_solver->local_search();
    }
}
