#include "sol.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace solver {

    Float qp_solver::eval_monomial(const monomial& mono) {
        int first_var = mono.m_vars[0];
        if (mono.is_linear) return mono.coeff * _cur_assignment[first_var];
        if (mono.is_multilinear) {
            return mono.coeff * _cur_assignment[first_var] * _cur_assignment[mono.m_vars[1]];
        }
        return mono.coeff * _cur_assignment[first_var] * _cur_assignment[first_var];
    }

    Float qp_solver::eval_constraint_value(polynomial_constraint* pcon) {
        Float sum = 0;
        for (const auto& mono : pcon->monomials) {
            sum += eval_monomial(mono);
        }
        return sum;
    }

    Float qp_solver::eval_objective_value() {
        Float sum = 0;
        for (const auto& mono : _object_monoials) {
            sum += eval_monomial(mono);
        }
        return sum;
    }

    bool qp_solver::is_value_legal_for_var(int var_idx, Float new_value) {
        if (!std::isfinite(new_value)) return false;
        var* v = &_vars[var_idx];
        if (v->has_lower && new_value < v->lower - eb) return false;
        if (v->has_upper && new_value > v->upper + eb) return false;
        if (v->is_bin) {
            if (std::abs(new_value - 0.0) > eb && std::abs(new_value - 1.0) > eb) return false;
        }
        if (v->is_int) {
            if (std::abs(new_value - std::round(new_value)) > 1e-8) return false;
        }
        return true;
    }

    bool qp_solver::validate_assignment(const vector<Float>& assignment, Float* max_violation) {
        Float max_viol = 0;
        bool valid = assignment.size() == _vars.size();

        if (valid) {
            for (size_t i = 0; i < _vars.size(); ++i) {
                if (!is_value_legal_for_var(static_cast<int>(i), assignment[i])) {
                    valid = false;
                    break;
                }
            }
        }

        if (valid) {
            for (const auto& pcon : _constraints) {
                Float value = 0;
                for (const auto& mono : pcon.monomials) {
                    int first_var = mono.m_vars[0];
                    Float term;
                    if (mono.is_linear) {
                        term = mono.coeff * assignment[first_var];
                    } else if (mono.is_multilinear) {
                        term = mono.coeff * assignment[first_var] * assignment[mono.m_vars[1]];
                    } else {
                        term = mono.coeff * assignment[first_var] * assignment[first_var];
                    }
                    value += term;
                }

                Float violation;
                if (!std::isfinite(value)) {
                    violation = std::numeric_limits<Float>::infinity();
                } else if (pcon.is_equal) {
                    violation = std::abs(value - pcon.bound);
                } else if (pcon.is_less) {
                    violation = std::max((Float)0.0, value - pcon.bound);
                } else {
                    violation = std::max((Float)0.0, pcon.bound - value);
                }
                max_viol = std::max(max_viol, violation);
                if (violation > eb) valid = false;
            }
        }

        if (max_violation != nullptr) *max_violation = max_viol;
        return valid;
    }

    bool qp_solver::recompute_current_solution_state() {
        _unsat_constraints.clear();
        _unbounded_constraints.clear();

        bool domains_valid = _cur_assignment.size() == _vars.size();
        if (domains_valid) {
            for (size_t i = 0; i < _vars.size(); ++i) {
                if (!is_value_legal_for_var(static_cast<int>(i), _cur_assignment[i])) {
                    domains_valid = false;
                    break;
                }
            }
        }

        for (auto& pcon : _constraints) {
            pcon.value = eval_constraint_value(&pcon);
            Float violation = constraint_violation(&pcon);
            pcon.is_sat = std::isfinite(pcon.value) && violation <= eb;
            if (!pcon.is_sat) {
                _unsat_constraints.insert(pcon.index);
                continue;
            }
            if (!pcon.is_equal && std::abs(pcon.value - pcon.bound) > eb) {
                _unbounded_constraints.insert(pcon.index);
            }
        }

        is_cur_feasible = domains_valid && _unsat_constraints.empty();
        return is_cur_feasible;
    }

    // Phase F: Stagnation & Budget Methods
    Float qp_solver::total_constraint_violation() {
        Float total_viol = 0;
        for (auto& c : _constraints) {
            total_viol += constraint_violation(&c);
        }
        return total_viol;
    }

    bool qp_solver::is_repair_stagnating() {
        if (is_cur_feasible) return false;
        return (_steps - last_violation_improve_step >= block_repair_stagnation_window);
    }

    bool qp_solver::is_objective_stagnating() {
        if (!is_cur_feasible) return false;
        return (_steps - last_best_update_step >= block_objective_stagnation_window);
    }

    bool qp_solver::block_time_budget_available() {
        double elapsed = TimeElapsed();
        if (elapsed <= 0) return true;
        return (block_time_spent / elapsed) <= block_time_budget_ratio;
    }

    bool qp_solver::should_try_block_repair() {
        if (!block_adaptive_trigger_enabled) return true;
        if (_steps < block_min_steps_before_enable) {
            block_skipped_by_min_steps_total++;
            return false;
        }
        if (_steps % block_trigger_interval != 0) {
            block_skipped_by_interval_total++;
            return false;
        }
        if (!is_repair_stagnating()) {
            block_skipped_non_stagnation_total++;
            return false;
        }
        if (!block_time_budget_available()) {
            block_skipped_by_time_budget_total++;
            return false;
        }
        block_triggered_by_repair_stagnation_total++;
        return true;
    }

    bool qp_solver::should_try_block_improve() {
        if (!block_adaptive_trigger_enabled) return true;
        if (block_repair_only_until_feasible && last_feasible_step == -1) return false;
        if (_steps < block_min_steps_before_enable) {
            block_skipped_by_min_steps_total++;
            return false;
        }
        if (_steps % block_trigger_interval != 0) {
            block_skipped_by_interval_total++;
            return false;
        }
        if (!is_objective_stagnating()) {
            block_skipped_non_stagnation_total++;
            return false;
        }
        if (!block_time_budget_available()) {
            block_skipped_by_time_budget_total++;
            return false;
        }
        block_triggered_by_objective_stagnation_total++;
        return true;
    }

    Float qp_solver::constraint_violation(polynomial_constraint* pcon) {
        Float val = eval_constraint_value(pcon);
        if (pcon->is_equal) {
            return std::abs(val - pcon->bound);
        }
        if (pcon->is_less) {
            return std::max((Float)0.0, val - pcon->bound);
        }
        return std::max((Float)0.0, pcon->bound - val);
    }

    // Phase G: Violation-Prioritized Safe Block Repair
    Float qp_solver::normalized_constraint_violation(int c_idx) {
        if (c_idx < 0 || c_idx >= (int)_constraints.size()) return 0;
        polynomial_constraint* pcon = &_constraints[c_idx];
        Float raw = constraint_violation(pcon);
        Float value = eval_constraint_value(pcon);
        Float denom = 1.0 + std::abs(pcon->bound) + std::abs(value);
        return raw / denom;
    }

    vector<int> qp_solver::select_repair_constraints_topk() {
        vector<int> repair_cons(_unsat_constraints.begin(), _unsat_constraints.end());
        if (!block_prioritized_repair_enabled || block_top_cons_num <= 0) {
            return repair_cons;
        }

        struct RepairConstraintPriority {
            int index;
            Float priority;
            Float raw_violation;
        };

        vector<RepairConstraintPriority> ranked;
        ranked.reserve(repair_cons.size());
        for (int c_idx : repair_cons) {
            Float raw = constraint_violation(&_constraints[c_idx]);
            ranked.push_back({
                c_idx,
                _constraints[c_idx].weight * normalized_constraint_violation(c_idx),
                raw
            });
        }
        std::sort(ranked.begin(), ranked.end(),
            [](const RepairConstraintPriority& lhs, const RepairConstraintPriority& rhs) {
                if (lhs.priority != rhs.priority) return lhs.priority > rhs.priority;
                if (lhs.raw_violation != rhs.raw_violation) {
                    return lhs.raw_violation > rhs.raw_violation;
                }
                return lhs.index < rhs.index;
            });

        size_t selected_size = std::min(ranked.size(), (size_t)block_top_cons_num);
        repair_cons.clear();
        repair_cons.reserve(selected_size);
        for (size_t i = 0; i < selected_size; ++i) {
            repair_cons.push_back(ranked[i].index);
        }

        block_repair_considered_constraints_total += ranked.size();
        block_repair_selected_constraints_total += repair_cons.size();
        block_repair_skipped_by_topk_total += ranked.size() - repair_cons.size();
        return repair_cons;
    }

    Float qp_solver::projected_constraint_violation(polynomial_constraint* pcon, const vector<int>& vars, const vector<Float>& shifts) {
        vector<Float> old_values;
        for (size_t i = 0; i < vars.size(); ++i) {
            old_values.push_back(_cur_assignment[vars[i]]);
            _cur_assignment[vars[i]] += shifts[i];
        }
        Float viol = constraint_violation(pcon);
        for (size_t i = 0; i < vars.size(); ++i) {
            _cur_assignment[vars[i]] = old_values[i];
        }
        return viol;
    }

    Float qp_solver::projected_obj_delta(const vector<int>& vars, const vector<Float>& shifts) {
        Float old_obj = eval_objective_value();
        vector<Float> old_values;
        for (size_t i = 0; i < vars.size(); ++i) {
            old_values.push_back(_cur_assignment[vars[i]]);
            _cur_assignment[vars[i]] += shifts[i];
        }
        Float new_obj = eval_objective_value();
        for (size_t i = 0; i < vars.size(); ++i) {
            _cur_assignment[vars[i]] = old_values[i];
        }
        return new_obj - old_obj;
    }

    qp_solver::BlockRejectReason qp_solver::diagnose_block_move(const block_move& mv) {
        if (mv.vars.empty()) return BlockRejectReason::EMPTY_BLOCK;
        if (mv.vars.size() != mv.shifts.size()) return BlockRejectReason::SIZE_MISMATCH;
        bool has_non_zero = false;
        unordered_set<int> unique_vars;
        for (size_t i = 0; i < mv.vars.size(); ++i) {
            int var_idx = mv.vars[i];
            Float shift = mv.shifts[i];
            if (unique_vars.find(var_idx) != unique_vars.end()) return BlockRejectReason::DUPLICATE_VAR;
            unique_vars.insert(var_idx);
            if (std::abs(shift) > eb) has_non_zero = true;
            Float new_value = _cur_assignment[var_idx] + shift;
            if (!is_value_legal_for_var(var_idx, new_value)) return BlockRejectReason::ILLEGAL_VALUE;
        }
        if (!has_non_zero) return BlockRejectReason::ZERO_SHIFT;
        return BlockRejectReason::NONE;
    }

    bool qp_solver::check_block_move(const block_move& mv) {
        return diagnose_block_move(mv) == BlockRejectReason::NONE;
    }

    void qp_solver::update_block_reject_stats(BlockRejectReason reason) {
        switch (reason) {
            case BlockRejectReason::EMPTY_BLOCK: block_fail_empty_total++; break;
            case BlockRejectReason::SIZE_MISMATCH: block_fail_size_mismatch_total++; break;
            case BlockRejectReason::DUPLICATE_VAR: block_fail_duplicate_var_total++; break;
            case BlockRejectReason::ZERO_SHIFT: block_fail_zero_shift_total++; break;
            case BlockRejectReason::ILLEGAL_VALUE: block_fail_illegal_value_total++; break;
            case BlockRejectReason::CHECK_VAR_SHIFT_FAILED: block_fail_check_var_shift_total++; break;
            case BlockRejectReason::EXECUTE_CONSISTENCY_FAILED: block_fail_consistency_total++; break;
            default: block_fail_unknown_total++; break;
        }
    }

    void qp_solver::print_block_failure_sample(
        const block_move& mv, BlockRejectReason reason, Float block_score, Float single_score, Float pair_score, const string& context
    ) {
        if (block_debug_fail_printed >= block_debug_fail_print_limit) return;
        block_debug_fail_printed++;
        
        std::cerr << "[BLOCK_FAIL_SAMPLE]" << std::endl;
        std::cerr << "context = " << context << std::endl;
        std::cerr << "step = " << _steps << std::endl;
        std::cerr << "reason = " << static_cast<int>(reason) << std::endl;
        std::cerr << "mv.score = " << block_score << std::endl;
        std::cerr << "single_score = " << single_score << std::endl;
        std::cerr << "pair_score = " << pair_score << std::endl;
        std::cerr << "cons_pos = " << mv.cons_pos << std::endl;
        std::cerr << "is_repair = " << mv.is_repair << std::endl;
        std::cerr << "vars:" << std::endl;
        for (size_t i = 0; i < mv.vars.size(); ++i) {
            int v = mv.vars[i];
            Float s = i < mv.shifts.size() ? mv.shifts[i] : 0;
            std::cerr << "  idx=" << v << ", name=" << _vars[v].name 
                      << ", cur=" << _cur_assignment[v] << ", shift=" << s 
                      << ", new=" << (_cur_assignment[v] + s) 
                      << ", is_bin=" << _vars[v].is_bin << ", is_int=" << _vars[v].is_int 
                      << ", lower=" << _vars[v].lower << ", upper=" << _vars[v].upper << std::endl;
        }
        std::cerr << "affected constraints:" << std::endl;
        unordered_set<int> affected_cons;
        for (int v : mv.vars) {
            for (int c : _vars[v].constraints) affected_cons.insert(c);
        }
        for (int c : affected_cons) {
            polynomial_constraint* pcon = &_constraints[c];
            Float old_v = pcon->value;
            Float proj_v = eval_constraint_value(pcon); // this is wrong if it's evaluated without changing assignment
            // actually we can compute new violation using projected_constraint_violation
            Float old_viol = constraint_violation(pcon);
            Float new_viol = projected_constraint_violation(pcon, mv.vars, mv.shifts);
            std::cerr << "  idx=" << c << ", bound=" << pcon->bound << ", is_equal=" << pcon->is_equal << ", is_less=" << pcon->is_less
                      << ", old_violation=" << old_viol << ", new_violation=" << new_viol << std::endl;
        }
    }

    bool qp_solver::execute_block_move(const block_move& mv) {
        block_execute_attempt_total++;
        BlockRejectReason reason = diagnose_block_move(mv);
        if (reason != BlockRejectReason::NONE) {
            block_execute_failed_total++;
            update_block_reject_stats(reason);
            print_block_failure_sample(mv, reason, mv.score, INT32_MIN, INT32_MIN, "execute_block_move");
            return false;
        }

        // Phase G: defend against stale or externally supplied repair candidates.
        if (mv.is_repair && block_safe_acceptance_enabled) {
            Float old_safe = 0;
            Float new_safe = 0;
            int newly_bad = 0;
            if (!safe_accept_block_repair(mv, &old_safe, &new_safe, &newly_bad)) {
                block_execute_failed_total++;
                return false;
            }
        }
        
        unordered_set<int> affected_cons;
        for (int v : mv.vars) {
            for (int c : _vars[v].constraints) affected_cons.insert(c);
        }

        Float obj_delta = projected_obj_delta(mv.vars, mv.shifts);
        Float old_viol_sum = 0;
        if (mv.is_repair) {
            unordered_set<int> affected = collect_affected_constraints(mv.vars);
            for (int c : affected) {
                old_viol_sum += constraint_violation(&_constraints[c]);
            }
        }
        
        for (size_t i = 0; i < mv.vars.size(); ++i) {
            int var_idx = mv.vars[i];
            Float shift = mv.shifts[i];
            Float new_val = _cur_assignment[var_idx] + shift;
            _cur_assignment[var_idx] = new_val;
            
            if (shift > eb) _vars[var_idx].last_pos_step = _steps;
            else if (shift < -eb) _vars[var_idx].last_neg_step = _steps;
            
            if (_vars[var_idx].recent_value != nullptr) {
                _vars[var_idx].recent_value->update(new_val);
            }
        }

        Float new_viol_sum = 0;
        recompute_current_solution_state();
        for (int c : affected_cons) {
            polynomial_constraint* pcon = &_constraints[c];
            if (mv.is_repair) {
                new_viol_sum += constraint_violation(pcon);
            }
        }
        
        if (is_cur_feasible) update_best_solution();
        
        if (mv.cons_pos != -1 && _constraint_tabu_enabled) {
            _constraints[mv.cons_pos].tabu_step = _steps + rand() % 10 + 3;
        }
        
        block_executed_total++;
        if (mv.is_repair) {
            block_repair_executed_total++;
            if (is_cur_feasible) block_repair_success_total++; 
            
            if (new_viol_sum < old_viol_sum - eb) {
                block_violation_improve_executed_total++;
            }
            block_violation_delta_sum += (new_viol_sum - old_viol_sum);
        } else {
            block_improve_executed_total++;
            if (is_cur_feasible) block_keep_feasible_executed_total++;
        }
        
        if (obj_delta < -eb) block_obj_improve_executed_total++;
        else if (obj_delta > eb) block_obj_worsen_executed_total++;
        block_obj_delta_sum += obj_delta;
        
        return true;
    }

    void qp_solver::add_unique_candidate_value(vector<Float>& values, int var_idx, Float new_value) {
        var& v = _vars[var_idx];
        if (v.has_lower && new_value < v.lower) return;
        if (v.has_upper && new_value > v.upper) return;
        if (v.is_bin && new_value != 0 && new_value != 1) return;
        if (v.is_int && std::abs(std::round(new_value) - new_value) > eb) return;

        Float shift = new_value - _cur_assignment[var_idx];
        if (std::abs(shift) < eb) return;

        for (Float val : values) {
            if (std::abs(val - shift) < eb) return;
        }
        values.push_back(shift);
    }

    vector<Float> qp_solver::compute_constraint_roots_for_var(polynomial_constraint* pcon, int var_idx) {
        vector<Float> roots;
        Float a = 0;
        if (pcon->var_coeff.find(var_idx) != pcon->var_coeff.end()) {
            a = pcon->var_coeff[var_idx].obj_quadratic_coeff;
        }
        
        Float cur_v = _cur_assignment[var_idx];
        
        // Find b by shifting cur_v slightly and measuring the change, since we have eval_constraint_value
        // wait, we can just do this mathematically or by using the linear coeffs if we iterate all of them.
        // It's easier and safer to use the actual coefficients in pcon.
        Float b = 0;
        if (pcon->var_coeff.find(var_idx) != pcon->var_coeff.end()) {
            b += pcon->var_coeff[var_idx].obj_constant_coeff;
            for (size_t k = 0; k < pcon->var_coeff[var_idx].obj_linear_coeff.size(); ++k) {
                int var_k = pcon->var_coeff[var_idx].obj_linear_coeff[k];
                Float cur_k = _cur_assignment[var_k];
                b += cur_k * pcon->var_coeff[var_idx].obj_linear_constant_coeff[k];
            }
        }
        // Also check if var_idx appears in other variables' linear_coeffs?
        // In this project, `var_coeff` stores ALL terms where this variable is involved, but for cross terms, 
        // does it store in BOTH variables' var_coeff?
        // Let's assume yes or use a finite difference to be 100% sure:
        // LHS(x) = a x^2 + b x + c.
        // We know LHS(cur) = pcon->value.
        // b = d(LHS)/dx at x=0. d(LHS)/dx at cur = 2*a*cur + b.
        // To find d(LHS)/dx at cur:
        _cur_assignment[var_idx] = cur_v + 1.0;
        Float val_plus_1 = eval_constraint_value(pcon);
        _cur_assignment[var_idx] = cur_v - 1.0;
        Float val_minus_1 = eval_constraint_value(pcon);
        _cur_assignment[var_idx] = cur_v; // restore
        
        // val_plus_1 - val_minus_1 = a(cur+1)^2 + b(cur+1) - (a(cur-1)^2 + b(cur-1)) = a(4*cur) + 2b
        // So b = (val_plus_1 - val_minus_1 - 4 * a * cur_v) / 2.0;
        b = (val_plus_1 - val_minus_1 - 4.0 * a * cur_v) / 2.0;
        
        Float rest = pcon->value - (a * cur_v * cur_v + b * cur_v);
        Float c = rest - pcon->bound;

        if (std::abs(a) < eb) {
            if (std::abs(b) > eb) {
                roots.push_back(-c / b);
            }
        } else {
            Float discriminant = b * b - 4 * a * c;
            if (discriminant >= -eb) { // Allow slight negative due to precision
                if (discriminant < 0) discriminant = 0;
                Float sqrt_d = std::sqrt(discriminant);
                roots.push_back((-b + sqrt_d) / (2 * a));
                roots.push_back((-b - sqrt_d) / (2 * a));
            }
        }
        return roots;
    }

    vector<vector<int>> qp_solver::select_candidate_blocks_from_constraint(polynomial_constraint* pcon, int max_k) {
        std::vector<std::pair<int, Float>> var_weights;
        for (auto& pair : pcon->var_coeff) {
            int v = pair.first;
            Float constant_coeff = pair.second.obj_constant_coeff;
            Float quadratic_coeff = pair.second.obj_quadratic_coeff;
            Float linear_sum = 0;
            for (Float val : pair.second.obj_linear_constant_coeff) linear_sum += std::abs(val);
            
            Float obj_presence_bonus = _vars[v].is_in_obj ? 10.0 : 0.0;
            Float unsat_related_bonus = _vars[v].constraints.size() * 1.0;

            Float w = std::abs(constant_coeff) 
                    + std::abs(quadratic_coeff) * (1.0 + std::abs(_cur_assignment[v]))
                    + linear_sum
                    + obj_presence_bonus
                    + unsat_related_bonus;
            var_weights.push_back({v, w});
        }
        std::sort(var_weights.begin(), var_weights.end(), [](const std::pair<int, Float>& a, const std::pair<int, Float>& b) {
            return a.second > b.second;
        });
        
        vector<vector<int>> blocks;
        if (var_weights.empty()) return blocks;

        vector<int> top_vars;
        for (size_t i = 0; i < var_weights.size() && i < (size_t)max_k; ++i) {
            top_vars.push_back(var_weights[i].first);
        }
        
        if (top_vars.size() >= 2) {
            blocks.push_back({top_vars[0], top_vars[1]});
        }
        if (top_vars.size() >= 3) {
            blocks.push_back({top_vars[0], top_vars[1], top_vars[2]});
        }
        if (block_max_size >= 4 && top_vars.size() >= 4) {
            blocks.push_back({top_vars[0], top_vars[1], top_vars[2], top_vars[3]});
        }
        
        if (!top_vars.empty()) {
            int v1 = top_vars[0];
            int best_coupled = -1;
            Float max_couple = 0;
            if (pcon->var_coeff.find(v1) != pcon->var_coeff.end()) {
                auto& vc = pcon->var_coeff[v1];
                for (size_t i = 0; i < vc.obj_linear_coeff.size(); ++i) {
                    int v2 = vc.obj_linear_coeff[i];
                    Float couple = std::abs(vc.obj_linear_constant_coeff[i]);
                    if (couple > max_couple) {
                        max_couple = couple;
                        best_coupled = v2;
                    }
                }
            }
            if (best_coupled != -1 && best_coupled != v1) {
                blocks.push_back({v1, best_coupled});
            }
        }
        
        return blocks;
    }

    bool qp_solver::compute_objective_stationary_point(int var_idx, Float& x_ext) {
        Float a = _vars[var_idx].obj_quadratic_coeff;
        Float b = _vars[var_idx].obj_constant_coeff;
        
        for (size_t k = 0; k < _vars[var_idx].obj_linear_coeff.size(); ++k) {
            int var_k = _vars[var_idx].obj_linear_coeff[k];
            b += _cur_assignment[var_k] * _vars[var_idx].obj_linear_constant_coeff[k];
        }
        
        if (std::abs(a) > eb) {
            x_ext = -b / (2 * a);
            return true;
        }
        return false;
    }

    vector<Float> qp_solver::generate_candidate_shifts_for_var(polynomial_constraint* pcon, int var_idx, bool repair_mode) {
        vector<Float> shifts;
        var& v = _vars[var_idx];
        Float cur = _cur_assignment[var_idx];

        if (!block_smart_candidate_enabled) {
            Float fixed_shifts[] = {1.0, -1.0, 0.1, -0.1};
            for (Float s : fixed_shifts) {
                add_unique_candidate_value(shifts, var_idx, cur + s);
            }
            return shifts;
        }

        if (v.is_bin) {
            Float new_val = (cur > 0.5) ? 0.0 : 1.0;
            add_unique_candidate_value(shifts, var_idx, new_val);
            return shifts;
        }

        if (v.has_lower) add_unique_candidate_value(shifts, var_idx, v.lower);
        if (v.has_upper) add_unique_candidate_value(shifts, var_idx, v.upper);

        add_unique_candidate_value(shifts, var_idx, cur + 1.0);
        add_unique_candidate_value(shifts, var_idx, cur - 1.0);
        if (!v.is_int) {
            add_unique_candidate_value(shifts, var_idx, cur + 0.1);
            add_unique_candidate_value(shifts, var_idx, cur - 0.1);
        }

        Float x_ext;
        if (compute_objective_stationary_point(var_idx, x_ext)) {
            if (v.is_int) {
                add_unique_candidate_value(shifts, var_idx, std::round(x_ext));
                add_unique_candidate_value(shifts, var_idx, std::floor(x_ext));
                add_unique_candidate_value(shifts, var_idx, std::ceil(x_ext));
            } else {
                add_unique_candidate_value(shifts, var_idx, x_ext);
            }
        }

        if (pcon) {
            vector<Float> roots = compute_constraint_roots_for_var(pcon, var_idx);
            for (Float root : roots) {
                if (v.is_int) {
                    if (!pcon->is_equal) {
                        add_unique_candidate_value(shifts, var_idx, std::floor(root));
                        add_unique_candidate_value(shifts, var_idx, std::ceil(root));
                        add_unique_candidate_value(shifts, var_idx, std::round(root));
                    }
                } else {
                    add_unique_candidate_value(shifts, var_idx, root);
                }
            }
            if (!v.is_int && roots.size() == 2 && pcon->is_less) {
                add_unique_candidate_value(shifts, var_idx, (roots[0] + roots[1]) / 2.0);
            }
        }
        return shifts;
    }

    vector<vector<Float>> qp_solver::generate_candidate_values_for_block(polynomial_constraint* pcon, const vector<int>& vars, bool repair_mode) {
        vector<vector<Float>> all_var_shifts;
        for (int v : vars) {
            vector<Float> shifts = generate_candidate_shifts_for_var(pcon, v, repair_mode);
            block_var_candidate_raw_total += shifts.size();
            
            vector<Float> legal_shifts;
            for (Float s : shifts) {
                if (is_value_legal_for_var(v, _cur_assignment[v] + s)) {
                    legal_shifts.push_back(s);
                }
            }
            block_var_candidate_legal_total += legal_shifts.size();
            
            if (legal_shifts.size() > (size_t)block_candidate_value_cap) {
                legal_shifts.resize(block_candidate_value_cap);
            }
            if (legal_shifts.empty()) return {}; 
            
            all_var_shifts.push_back(legal_shifts);
        }
        
        vector<vector<Float>> cartesian_product;
        vector<Float> current_combo;
        auto backtrack = [&](auto& self, int depth) -> void {
            if (cartesian_product.size() >= (size_t)block_enum_cap) return;
            if (depth == vars.size()) {
                cartesian_product.push_back(current_combo);
                return;
            }
            for (Float shift : all_var_shifts[depth]) {
                current_combo.push_back(shift);
                self(self, depth + 1);
                current_combo.pop_back();
            }
        };
        backtrack(backtrack, 0);
        return cartesian_product;
    }

    unordered_set<int> qp_solver::collect_affected_constraints(const vector<int>& vars) {
        unordered_set<int> affected;
        for (int v : vars) {
            for (int c : _vars[v].constraints) {
                affected.insert(c);
            }
        }
        return affected;
    }

    bool qp_solver::safe_accept_block_repair(
        const block_move& mv,
        Float* old_viol_sum,
        Float* new_viol_sum,
        int* newly_violated_sat_count
    ) {
        if (old_viol_sum != nullptr) *old_viol_sum = 0;
        if (new_viol_sum != nullptr) *new_viol_sum = 0;
        if (newly_violated_sat_count != nullptr) *newly_violated_sat_count = 0;

        if (!block_safe_acceptance_enabled || !mv.is_repair) return true;

        block_safe_accept_checked_total++;
        Float old_sum = 0;
        Float new_sum = 0;
        int newly_violated = 0;
        unordered_set<int> affected = collect_affected_constraints(mv.vars);
        for (int c_idx : affected) {
            polynomial_constraint* pcon = &_constraints[c_idx];
            Float old_v = constraint_violation(pcon);
            Float new_v = projected_constraint_violation(pcon, mv.vars, mv.shifts);
            old_sum += old_v;
            new_sum += new_v;
            if (old_v <= eb && new_v > eb) newly_violated++;
        }

        if (old_viol_sum != nullptr) *old_viol_sum = old_sum;
        if (new_viol_sum != nullptr) *new_viol_sum = new_sum;
        if (newly_violated_sat_count != nullptr) {
            *newly_violated_sat_count = newly_violated;
        }

        if (newly_violated > 0) {
            block_safe_rejected_total++;
            block_safe_rejected_new_sat_violation_total++;
            return false;
        }

        Float gain = old_sum - new_sum;
        Float min_gain = std::max((Float)eb,
            block_safe_min_relative_gain * (1.0 + std::abs(old_sum)));
        if (gain <= min_gain) {
            block_safe_rejected_total++;
            block_safe_rejected_no_violation_gain_total++;
            if (new_sum > old_sum + eb) block_safe_harmful_prevented_total++;
            return false;
        }
        return true;
    }

    Float qp_solver::calculate_score_block_mix(const block_move& mv, bool repair_mode) {
        if (mv.vars.empty() || mv.cons_pos == -1) return INT32_MIN;
        if (repair_mode && !safe_accept_block_repair(mv, nullptr, nullptr, nullptr)) {
            return INT32_MIN;
        }
        
        polynomial_constraint* pcon = &_constraints[mv.cons_pos];
        
        if (!block_normalized_score_enabled) {
            Float obj_delta = projected_obj_delta(mv.vars, mv.shifts);
            Float affected_score = 0;
            unordered_set<int> affected = collect_affected_constraints(mv.vars);
            for (int c_idx : affected) {
                if (c_idx == mv.cons_pos) continue;
                if (_unsat_constraints.find(c_idx) == _unsat_constraints.end()) continue; // simple version only for unsat
                Float cv_old = constraint_violation(&_constraints[c_idx]);
                Float cv_new = projected_constraint_violation(&_constraints[c_idx], mv.vars, mv.shifts);
                affected_score += 1000.0 * _constraints[c_idx].weight * (cv_old - cv_new);
            }
            if (repair_mode) {
                Float old_viol = constraint_violation(pcon);
                Float new_viol = projected_constraint_violation(pcon, mv.vars, mv.shifts);
                return block_repair_scale * pcon->weight * (old_viol - new_viol) + affected_score - _object_weight * obj_delta;
            } else {
                Float infeas_penalty = 0;
                for (int c_idx : affected) {
                    if (_unsat_constraints.find(c_idx) == _unsat_constraints.end()) {
                        Float cv_new = projected_constraint_violation(&_constraints[c_idx], mv.vars, mv.shifts);
                        if (cv_new > eb) {
                            infeas_penalty += 1000000.0;
                        }
                    }
                }
                return -_object_weight * obj_delta - infeas_penalty;
            }
        }

        unordered_set<int> affected = collect_affected_constraints(mv.vars);
        Float obj_delta = projected_obj_delta(mv.vars, mv.shifts);
        Float current_obj = eval_objective_value();
        Float normalized_obj_delta = obj_delta / (1.0 + std::abs(current_obj));

        if (repair_mode) {
            Float old_viol = constraint_violation(pcon);
            Float new_viol = projected_constraint_violation(pcon, mv.vars, mv.shifts);
            Float normalized_improve = (old_viol - new_viol) / (1.0 + std::abs(old_viol));
            
            Float affected_constraint_score = 0;
            for (int c_idx : affected) {
                if (c_idx == mv.cons_pos) continue;
                polynomial_constraint* aff_pcon = &_constraints[c_idx];
                Float cv_old = constraint_violation(aff_pcon);
                Float cv_new = projected_constraint_violation(aff_pcon, mv.vars, mv.shifts);
                // normalized improvement for affected constraint
                if (cv_old > eb || cv_new > eb) { // if it was unsat or becomes unsat
                    Float norm_aff_improv = (cv_old - cv_new) / (1.0 + std::abs(cv_old));
                    affected_constraint_score += aff_pcon->weight * norm_aff_improv;
                }
            }
            
            Float score = block_repair_scale * pcon->weight * normalized_improve 
                        + affected_constraint_score 
                        - _object_weight * normalized_obj_delta;
            return score;
        } else {
            // feasible improvement mode
            for (int c_idx : affected) {
                if (_unsat_constraints.find(c_idx) == _unsat_constraints.end()) { // it is sat
                    Float cv_new = projected_constraint_violation(&_constraints[c_idx], mv.vars, mv.shifts);
                    if (cv_new > eb) { // becomes unsat
                        return INT32_MIN;
                    }
                }
            }
            // if maintaining feasibility, score is obj delta
            if (obj_delta >= 0) return INT32_MIN;
            Float score = -_object_weight * normalized_obj_delta;
            return score;
        }
    }

    void qp_solver::insert_block_operations_mix(bool repair_mode) {
        _operation_vars_block.clear();
        if (repair_mode && !block_repair_enabled) return;
        vector<int> repair_cons;
        if (repair_mode) repair_cons = select_repair_constraints_topk();
        if (repair_mode) {
            for (int c_idx : repair_cons) {
                polynomial_constraint* pcon = &_constraints[c_idx];
                vector<vector<int>> blocks = select_candidate_blocks_from_constraint(pcon, block_max_size);
                for (const auto& vars : blocks) {
                    if (vars.empty()) continue;
                    auto combos = generate_candidate_values_for_block(pcon, vars, repair_mode);
                    block_combo_generated_total += combos.size();
                    for (const auto& shifts : combos) {
                        block_move mv;
                        mv.vars = vars;
                        mv.shifts = shifts;
                        mv.cons_pos = c_idx;
                        mv.is_repair = true;
                        block_candidates_total++;
                        if (check_block_move(mv)) {
                            block_combo_after_check_total++;
                            mv.score = calculate_score_block_mix(mv, repair_mode);
                            if (mv.score > 0) block_combo_score_positive_total++;
                            if (mv.score > INT32_MIN) {
                                _operation_vars_block.push_back(mv);
                            }
                        } else {
                            block_rejected_by_check_total++;
                        }
                    }
                }
            }
        } else {
            if (!block_improve_enabled) return;
            for (int c_idx = 0; c_idx < _constraints.size(); ++c_idx) {
                polynomial_constraint* pcon = &_constraints[c_idx];
                vector<vector<int>> blocks = select_candidate_blocks_from_constraint(pcon, block_max_size);
                for (const auto& vars : blocks) {
                    if (vars.empty()) continue;
                    auto combos = generate_candidate_values_for_block(pcon, vars, repair_mode);
                    block_combo_generated_total += combos.size();
                    for (const auto& shifts : combos) {
                        block_move mv;
                        mv.vars = vars;
                        mv.shifts = shifts;
                        mv.cons_pos = c_idx;
                        mv.is_repair = false;
                        block_candidates_total++;
                        if (check_block_move(mv)) {
                            block_combo_after_check_total++;
                            mv.score = calculate_score_block_mix(mv, repair_mode);
                            if (mv.score > 0) block_combo_score_positive_total++;
                            if (mv.score > INT32_MIN) {
                                _operation_vars_block.push_back(mv);
                            }
                        } else {
                            block_rejected_by_check_total++;
                        }
                    }
                }
            }
        }
    }

    void qp_solver::select_best_operation_block_mix(block_move& best_mv, Float& best_score) {
        best_score = INT32_MIN;
        for (const auto& mv : _operation_vars_block) {
            if (mv.score > best_score) {
                best_score = mv.score;
                best_mv = mv;
            } else if (mv.score == best_score) {
                // tie-breaking
                if (rand() % 2 == 0) {
                    best_mv = mv;
                }
            }
        }
    }

    bool qp_solver::debug_check_solution_consistency(const string& tag) {
#ifndef DEBUG
        return true;
#endif
        bool ok = true;
        for (int i = 0; i < _var_num; ++i) {
            Float val = _cur_assignment[i];
            if (!is_value_legal_for_var(i, val)) {
                std::cerr << "[" << tag << "] step " << _steps << " Var " << i << " illegal value: " << val << std::endl;
                ok = false;
            }
        }
        
        unordered_set<int> real_unsat;
        for (int i = 0; i < static_cast<int>(_constraints.size()); ++i) {
            polynomial_constraint* pcon = &_constraints[i];
            Float recomputed_val = eval_constraint_value(pcon);
            if (std::abs(pcon->value - recomputed_val) > 1e-4) {
                std::cerr << "[" << tag << "] step " << _steps << " Cons " << i << " value mismatch. Cached: " << pcon->value << ", Recomputed: " << recomputed_val << std::endl;
                ok = false;
            }
            Float viol = constraint_violation(pcon);
            bool recomputed_sat = viol <= eb;
            if (pcon->is_sat != recomputed_sat) {
                std::cerr << "[" << tag << "] step " << _steps << " Cons " << i << " is_sat mismatch. Cached: " << pcon->is_sat << ", Recomputed: " << recomputed_sat << std::endl;
                ok = false;
            }
            if (!recomputed_sat) real_unsat.insert(i);
        }
        
        if (real_unsat.size() != _unsat_constraints.size()) {
            std::cerr << "[" << tag << "] step " << _steps << " unsat_constraints size mismatch. Cached: " << _unsat_constraints.size() << ", Recomputed: " << real_unsat.size() << std::endl;
            ok = false;
        }
        
        bool recomputed_feasible = real_unsat.empty();
        if (is_cur_feasible != recomputed_feasible) {
            std::cerr << "[" << tag << "] step " << _steps << " is_cur_feasible mismatch. Cached: " << is_cur_feasible << ", Recomputed: " << recomputed_feasible << std::endl;
            ok = false;
        }
        
        return ok;
    }

    void qp_solver::print_block_statistics() {
        if (!block_move_enabled) return;
        std::cerr << "block stats:" << std::endl;
        std::cerr << "  enabled = " << block_move_enabled << std::endl;
        std::cerr << "  repair_enabled = " << block_repair_enabled << std::endl;
        std::cerr << "  improve_enabled = " << block_improve_enabled << std::endl;
        std::cerr << "  smart_candidate = " << block_smart_candidate_enabled << std::endl;
        std::cerr << "  normalized_score = " << block_normalized_score_enabled << std::endl;

        // Phase G output
        std::cerr << "  prioritized_repair_enabled = " << block_prioritized_repair_enabled << std::endl;
        std::cerr << "  safe_acceptance_enabled = " << block_safe_acceptance_enabled << std::endl;
        std::cerr << "  safe_min_relative_gain = " << block_safe_min_relative_gain << std::endl;
        std::cerr << "  repair_considered_constraints_total = " << block_repair_considered_constraints_total << std::endl;
        std::cerr << "  repair_selected_constraints_total = " << block_repair_selected_constraints_total << std::endl;
        std::cerr << "  repair_skipped_by_topk_total = " << block_repair_skipped_by_topk_total << std::endl;
        std::cerr << "  safe_accept_checked_total = " << block_safe_accept_checked_total << std::endl;
        std::cerr << "  safe_rejected_total = " << block_safe_rejected_total << std::endl;
        std::cerr << "  safe_rejected_no_violation_gain_total = " << block_safe_rejected_no_violation_gain_total << std::endl;
        std::cerr << "  safe_rejected_new_sat_violation_total = " << block_safe_rejected_new_sat_violation_total << std::endl;
        std::cerr << "  safe_harmful_prevented_total = " << block_safe_harmful_prevented_total << std::endl;

        // Phase H output
        std::cerr << "  legacy_seed_schedule_enabled = " << legacy_seed_schedule_enabled << std::endl;
        std::cerr << "  strict_incumbent_validation_enabled = " << strict_incumbent_validation_enabled << std::endl;
        std::cerr << "  broad_escape_pool_enabled = " << broad_escape_pool_enabled << std::endl;
        std::cerr << "  serial_diversification_enabled = " << serial_diversification_enabled << std::endl;
        std::cerr << "  serial_diversification_time_ratio = " << serial_diversification_time_ratio << std::endl;
        std::cerr << "  serial_diversification_max_density = " << serial_diversification_max_density << std::endl;
        std::cerr << "  serial_diversification_kick_cap = " << serial_diversification_kick_cap << std::endl;
        std::cerr << "  serial_diversification_long_max_restarts = " << serial_diversification_long_max_restarts << std::endl;
        std::cerr << "  serial_diversification_long_run_threshold = " << serial_diversification_long_run_threshold << std::endl;
        std::cerr << "  serial_diversification_long_first_ratio = " << serial_diversification_long_first_ratio << std::endl;
        std::cerr << "  serial_diversification_long_interval_ratio = " << serial_diversification_long_interval_ratio << std::endl;
        std::cerr << "  serial_diversification_min_stagnation_ratio = " << serial_diversification_min_stagnation_ratio << std::endl;
        std::cerr << "  serial_diversification_long_kick_growth = " << serial_diversification_long_kick_growth << std::endl;
        std::cerr << "  serial_diversification_total = " << serial_diversification_total << std::endl;
        std::cerr << "  qubo_incremental_gain_enabled = " << qubo_incremental_gain_enabled << std::endl;
        std::cerr << "  qubo_gain_cache_active = " << qubo_gain_cache_active << std::endl;
        std::cerr << "  qubo_gain_cache_hits_total = " << qubo_gain_cache_hits_total << std::endl;
        std::cerr << "  qubo_gain_cache_rebuild_total = " << qubo_gain_cache_rebuild_total << std::endl;
        std::cerr << "  qubo_gain_cache_neighbor_updates_total = " << qubo_gain_cache_neighbor_updates_total << std::endl;
        std::cerr << "  qubo_gain_cache_mismatch_total = " << qubo_gain_cache_mismatch_total << std::endl;
        
        // Phase F output
        std::cerr << "  adaptive_trigger_enabled = " << block_adaptive_trigger_enabled << std::endl;
        std::cerr << "  repair_stagnation_window = " << block_repair_stagnation_window << std::endl;
        std::cerr << "  objective_stagnation_window = " << block_objective_stagnation_window << std::endl;
        std::cerr << "  block_trigger_interval = " << block_trigger_interval << std::endl;
        std::cerr << "  block_time_budget_ratio = " << block_time_budget_ratio << std::endl;
        std::cerr << "  block_time_spent = " << block_time_spent << std::endl;
        std::cerr << "  block_disabled_unconstrained_total = " << block_disabled_unconstrained_total << std::endl;
        std::cerr << "  block_skipped_non_stagnation_total = " << block_skipped_non_stagnation_total << std::endl;
        std::cerr << "  block_triggered_by_repair_stagnation_total = " << block_triggered_by_repair_stagnation_total << std::endl;
        std::cerr << "  block_triggered_by_objective_stagnation_total = " << block_triggered_by_objective_stagnation_total << std::endl;
        std::cerr << "  block_skipped_by_time_budget_total = " << block_skipped_by_time_budget_total << std::endl;
        std::cerr << "  block_skipped_by_interval_total = " << block_skipped_by_interval_total << std::endl;
        std::cerr << "  block_repair_disabled_easy_instance_total = " << block_repair_disabled_easy_instance_total << std::endl;

        // Phase F-2 output
        std::cerr << "  block_min_steps_before_enable = " << block_min_steps_before_enable << std::endl;
        std::cerr << "  block_skipped_by_min_steps_total = " << block_skipped_by_min_steps_total << std::endl;
        std::cerr << "  block_disable_repair_after_first_feasible = " << block_disable_repair_after_first_feasible << std::endl;
        std::cerr << "  block_repair_disabled_after_first_feasible_total = " << block_repair_disabled_after_first_feasible_total << std::endl;

        std::cerr << "  seed = " << seed_num << std::endl;
        std::cerr << "  max_size = " << block_max_size << std::endl;
        std::cerr << "  top_cons_num = " << block_top_cons_num << std::endl;
        std::cerr << "  candidate_value_cap = " << block_candidate_value_cap << std::endl;
        std::cerr << "  enum_cap = " << block_enum_cap << std::endl;
        std::cerr << "  candidates_total = " << block_candidates_total << std::endl;
        std::cerr << "  var_candidate_raw_total = " << block_var_candidate_raw_total << std::endl;
        std::cerr << "  var_candidate_legal_total = " << block_var_candidate_legal_total << std::endl;
        std::cerr << "  combo_generated_total = " << block_combo_generated_total << std::endl;
        std::cerr << "  combo_after_check_total = " << block_combo_after_check_total << std::endl;
        std::cerr << "  combo_score_positive_total = " << block_combo_score_positive_total << std::endl;
        std::cerr << "  selected_total = " << block_selected_total << std::endl;
        std::cerr << "  execute_attempt_total = " << block_execute_attempt_total << std::endl;
        std::cerr << "  executed_total = " << block_executed_total << std::endl;
        std::cerr << "  repair_executed_total = " << block_repair_executed_total << std::endl;
        std::cerr << "  improve_executed_total = " << block_improve_executed_total << std::endl;
        std::cerr << "  repair_success_total = " << block_repair_success_total << std::endl;
        std::cerr << "  best_update_total = " << block_best_update_total << std::endl;
        std::cerr << "  obj_improve_executed_total = " << block_obj_improve_executed_total << std::endl;
        std::cerr << "  obj_worsen_executed_total = " << block_obj_worsen_executed_total << std::endl;
        std::cerr << "  violation_improve_executed_total = " << block_violation_improve_executed_total << std::endl;
        std::cerr << "  keep_feasible_executed_total = " << block_keep_feasible_executed_total << std::endl;
        std::cerr << "  feasibility_recheck_rejected_total = " << feasibility_recheck_rejected_total << std::endl;
        std::cerr << "  obj_delta_sum = " << block_obj_delta_sum << std::endl;
        std::cerr << "  violation_delta_sum = " << block_violation_delta_sum << std::endl;
        std::cerr << "  rejected_by_check_total = " << block_rejected_by_check_total << std::endl;
        std::cerr << "  zero_shift_rejected_total = " << block_zero_shift_rejected_total << std::endl;
        std::cerr << "  best_score_seen = " << block_best_score_seen << std::endl;
        std::cerr << "  execute_attempt_total = " << block_execute_attempt_total << std::endl;
        std::cerr << "  execute_failed_total = " << block_execute_failed_total << std::endl;
        std::cerr << "  fail_empty_total = " << block_fail_empty_total << std::endl;
        std::cerr << "  fail_size_mismatch_total = " << block_fail_size_mismatch_total << std::endl;
        std::cerr << "  fail_duplicate_var_total = " << block_fail_duplicate_var_total << std::endl;
        std::cerr << "  fail_zero_shift_total = " << block_fail_zero_shift_total << std::endl;
        std::cerr << "  fail_illegal_value_total = " << block_fail_illegal_value_total << std::endl;
        std::cerr << "  fail_check_var_shift_total = " << block_fail_check_var_shift_total << std::endl;
        std::cerr << "  fail_consistency_total = " << block_fail_consistency_total << std::endl;
        std::cerr << "  fail_unknown_total = " << block_fail_unknown_total << std::endl;
    }

} // namespace solver
