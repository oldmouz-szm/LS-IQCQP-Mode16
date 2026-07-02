#include "sol.h"
namespace solver
{

    bool qp_solver::fps_move()
    {
        return false;
        unordered_set<int> first_vars;
        vector<int> rand_monos;
        Float obj_delta;
        int mono_pos;
        monomial * mono;
        Float cur_value;
        Float change_value;
        Float value_1, value_2;
        Float change_value_fps_1, change_value_fps_2, score_fps_1, score_fps_2, best_score_fps = INT32_MIN;
        int best_var_1, best_var_2, s_var;
        for (int rand_times = 0; rand_times < 40; rand_times++) //40
        {
            mono_pos = rand() % _object_monoials.size();
            mono = & (_object_monoials[mono_pos]);
            obj_delta = pro_mono(*mono);
            if (mono->coeff > 0) 
            {
                if (obj_delta > 0)
                {
                    for (int var_pos : mono->m_vars)
                    {
                        cur_value = _cur_assignment[var_pos];
                        change_value = (cur_value == 0) ? 1 : -1;
                        if (check_var_shift_bool(var_pos, change_value, true))
                        {
                            first_vars.insert(var_pos);
                        }
                    }
                }
                else if (mono->is_multilinear)//控制x2的空间变大
                {
                    value_1 = _cur_assignment[mono->m_vars[0]];
                    value_2 = _cur_assignment[mono->m_vars[1]];
                    if (value_1 != value_2)
                    {
                        if (value_1 == 0)
                        {
                            change_value = (value_2 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[1], change_value, true))
                            {
                                first_vars.insert(mono->m_vars[1]);
                            }
                        }
                        else
                        {
                            change_value = (value_1 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[0], change_value, true))
                            {
                                first_vars.insert(mono->m_vars[0]);
                            }
                        }
                    }
                }
            }
            else if (mono->coeff < 0 && obj_delta == 0) 
            {
                if (!mono->is_multilinear)
                {
                    for (int var_pos : mono->m_vars)
                    {
                        cur_value = _cur_assignment[var_pos];
                        change_value = (cur_value == 0) ? 1 : -1;
                        if (check_var_shift_bool(var_pos, change_value, true))
                        {
                            first_vars.insert(var_pos);
                        }
                    }
                }
                else
                {
                    value_1 = _cur_assignment[mono->m_vars[0]];
                    value_2 = _cur_assignment[mono->m_vars[1]];
                    if (value_1 != value_2)
                    {
                        if (value_1 == 0)
                        {
                            change_value = (value_1 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[0], change_value, true))
                            {
                                first_vars.insert(mono->m_vars[0]);
                            }
                        }
                        else
                        {
                            change_value = (value_2 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[1], change_value, true))
                            {
                                first_vars.insert(mono->m_vars[1]);
                            }
                        }
                    }
                    else 
                    {
                        for (int var_pos : mono->m_vars)
                        {
                            cur_value = _cur_assignment[var_pos];
                            change_value = (cur_value == 0) ? 1 : -1;
                            if (check_var_shift_bool(var_pos, change_value, true))
                            {
                                first_vars.insert(var_pos);
                            }
                        }
                    }
                }
            }
        }
        for (auto f_var : first_vars)
        {
            change_value_fps_1 = (_cur_assignment[f_var] == 0) ? 1 : -1;
            score_fps_1 = calculate_score_no_cons(f_var, change_value_fps_1);
            _cur_assignment[f_var] += change_value_fps_1;
            insert_operation_no_cons();
            select_best_operation_no_cons(s_var, change_value_fps_2, score_fps_2);
            if (score_fps_2 > 0 && score_fps_1 + score_fps_2 > 0)
            {
                // best_score_fps = score_fps_1 + score_fps_2;
                best_var_1 = f_var;
                best_var_2 = s_var;
                _cur_assignment[f_var] -= change_value_fps_1;
                Float best_change_value_1 = (_cur_assignment[best_var_1] == 0) ? 1 : -1;
                Float best_change_value_2 = (_cur_assignment[best_var_2] == 0) ? 1 : -1;
                execute_critical_move_no_cons(best_var_1, best_change_value_1);
                execute_critical_move_no_cons(best_var_2, best_change_value_2);
                return true;
            }
            _cur_assignment[f_var] -= change_value_fps_1;
        }
        return false;
        if (best_score_fps > 0)
        {
            // cout << "2 flip " << best_var_1 << "   " << best_var_2 << endl; 
            // cout << best_score_fps << endl;
            Float best_change_value_1 = (_cur_assignment[best_var_1] == 0) ? 1 : -1;
            Float best_change_value_2 = (_cur_assignment[best_var_2] == 0) ? 1 : -1;
            execute_critical_move_no_cons(best_var_1, best_change_value_1);
            execute_critical_move_no_cons(best_var_2, best_change_value_2);
            return true;
        }
        return false;
    }

    Float qp_solver::calculate_coeff_in_obj(int var_pos)
    {
        var * obj_var = & (_vars[var_pos]);
        Float linear_coeff_value = obj_var->obj_constant_coeff;
        Float li_var_coeff;
        int li_var_idx;
        for (int linear_pos = 0; linear_pos < obj_var->obj_linear_coeff.size(); linear_pos++)
        {
            li_var_idx = obj_var->obj_linear_coeff[linear_pos];
            li_var_coeff = obj_var->obj_linear_constant_coeff[linear_pos];
            linear_coeff_value += _cur_assignment[li_var_idx] * li_var_coeff;
        }
        linear_coeff_value += obj_var->obj_quadratic_coeff;
        return linear_coeff_value;
    }

    bool qp_solver::two_flip_no_cons()
    {
        return false;
        unordered_set<int> rand_monos;
        Float obj_delta;
        int mono_pos;
        monomial * mono;
        Float cur_value;
        Float change_value_1, change_value_2;
        int var_1, var_2;
        Float value_1, value_2;
        Float cur_delta_1, cur_delta_2;
        Float best_var_1, best_var_2;
        Float best_descent = INT32_MAX;
        Float delta_mono;
        for (int rand_times = 0; rand_times < 40; rand_times++)
        {
            mono_pos = rand() % _object_monoials.size();
            if (rand_monos.find(mono_pos) == rand_monos.end()) rand_monos.insert(mono_pos);
            else continue;
            mono = & (_object_monoials[mono_pos]);
            if (mono->m_vars.size() == 2)
            {
                var_1 = mono->m_vars[0];
                var_2 = mono->m_vars[1];
                value_1 = _cur_assignment[var_1];
                value_2 = _cur_assignment[var_2];
                change_value_1 = (value_1 == 0) ? 1 : -1;
                change_value_2 = (value_2 == 0) ? 1 : -1;
                if (value_1 == value_2) delta_mono = mono->coeff;
                else delta_mono = - mono->coeff;
                if (!check_var_shift_bool(var_1, change_value_1, false)) continue;
                if (!check_var_shift_bool(var_2, change_value_2, false)) continue;
                if (_cur_delta[var_1] == INT32_MIN) //这里不能是-1
                {
                    cur_delta_1 = calculate_coeff_in_obj(var_1) * (change_value_1);
                    _cur_delta[var_1] = cur_delta_1;
                }
                else cur_delta_1 = _cur_delta[var_1];
                if (_cur_delta[var_2] == INT32_MIN)
                {
                    cur_delta_2 = calculate_coeff_in_obj(var_2) * (change_value_2);
                    _cur_delta[var_2] = cur_delta_2;
                }
                else cur_delta_2 = _cur_delta[var_2];
                // if (_cur_delta[var_1] < 0)
                // {
                //     Float best_change_value_1 = (_cur_assignment[var_1] == 0) ? 1 : -1;
                //     execute_critical_move_no_cons(var_1, best_change_value_1);
                //     return true;
                // }
                // else if (_cur_delta[var_2] < 0)
                // {
                //     Float best_change_value_2 = (_cur_assignment[var_2] == 0) ? 1 : -1;
                //     execute_critical_move_no_cons(var_2, best_change_value_2);
                //     return true;
                // }
                cur_value = delta_mono + cur_delta_1 + cur_delta_2;
                if (cur_value < best_descent && cur_value < cur_delta_1 && cur_value < cur_delta_2)
                {//这里再改改i
                    best_descent = delta_mono + cur_delta_1 + cur_delta_2;
                    best_var_1 = var_1;
                    best_var_2 = var_2;
                } 
            }
        }
        // cout << best_descent << endl;
        if (best_descent <= -1)
        {
            // cout << "2 flip " << best_var_1 << "   " << best_var_2 << endl; 
            // cout << best_descent << endl;
            Float best_change_value_1 = (_cur_assignment[best_var_1] == 0) ? 1 : -1;
            Float best_change_value_2 = (_cur_assignment[best_var_2] == 0) ? 1 : -1;
            execute_critical_move_no_cons(best_var_1, best_change_value_1);
            execute_critical_move_no_cons(best_var_2, best_change_value_2);
            return true;
        }
        else return false;
    }

    void qp_solver::random_walk_no_cons()
    {
        vector<int> rand_monos;
        Float obj_delta;
        int mono_pos;
        monomial * mono;
        Float cur_value;
        Float change_value;
        Float value_1, value_2;
        _operation_vars_sub.clear();
        _operation_value_sub.clear();
        for (int rand_times = 0; rand_times < 40; rand_times++) //40
        {
            mono_pos = rand() % _object_monoials.size();
            mono = & (_object_monoials[mono_pos]);
            obj_delta = pro_mono(*mono);
            if (mono->coeff > 0) 
            {
                if (obj_delta > 0)
                {
                    for (int var_pos : mono->m_vars)
                    {
                        cur_value = _cur_assignment[var_pos];
                        change_value = (cur_value == 0) ? 1 : -1;
                        if (check_var_shift_bool(var_pos, change_value, true))
                        {
                            _operation_vars_sub.push_back(var_pos);
                            _operation_value_sub.push_back(change_value);
                        }
                    }
                }
                else if (mono->is_multilinear)//控制x2的空间变大
                {
                    value_1 = _cur_assignment[mono->m_vars[0]];
                    value_2 = _cur_assignment[mono->m_vars[1]];
                    if (value_1 != value_2)
                    {
                        if (value_1 == 0)
                        {
                            change_value = (value_2 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[1], change_value, true))
                            {
                                _operation_vars_sub.push_back(mono->m_vars[1]);
                                _operation_value_sub.push_back(change_value);
                            }
                        }
                        else
                        {
                            change_value = (value_1 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[0], change_value, true))
                            {
                                _operation_vars_sub.push_back(mono->m_vars[0]);
                                _operation_value_sub.push_back(change_value);
                            }
                        }
                    }
                }
            }
            else if (mono->coeff < 0 && obj_delta == 0) 
            {
                if (!mono->is_multilinear)
                {
                    for (int var_pos : mono->m_vars)
                    {
                        cur_value = _cur_assignment[var_pos];
                        change_value = (cur_value == 0) ? 1 : -1;
                        if (check_var_shift_bool(var_pos, change_value, true))
                        {
                            _operation_vars_sub.push_back(var_pos);
                            _operation_value_sub.push_back(change_value);
                        }
                    }
                }
                else
                {
                    value_1 = _cur_assignment[mono->m_vars[0]];
                    value_2 = _cur_assignment[mono->m_vars[1]];
                    if (value_1 != value_2)
                    {
                        if (value_1 == 0)
                        {
                            change_value = (value_1 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[0], change_value, true))
                            {
                                _operation_vars_sub.push_back(mono->m_vars[0]);
                                _operation_value_sub.push_back(change_value);
                            }
                        }
                        else
                        {
                            change_value = (value_2 == 0) ? 1 : -1;
                            if (check_var_shift_bool(mono->m_vars[1], change_value, true))
                            {
                                _operation_vars_sub.push_back(mono->m_vars[1]);
                                _operation_value_sub.push_back(change_value);
                            }
                        }
                    }
                    else 
                    {
                        for (int var_pos : mono->m_vars)
                        {
                            cur_value = _cur_assignment[var_pos];
                            change_value = (cur_value == 0) ? 1 : -1;
                            if (check_var_shift_bool(var_pos, change_value, true))
                            {
                                _operation_vars_sub.push_back(var_pos);
                                _operation_value_sub.push_back(change_value);
                            }
                        }
                    }
                }
            }
        }
        int var_pos;
        Float change_value_2, score;
        select_best_operation_no_cons(var_pos, change_value_2, score);
        // cout << score << endl;
        if (score != INT32_MIN) execute_critical_move_no_cons(var_pos, change_value_2);
        else 
        {
            int rand_idx = rand() % _vars.size();
            // if (_cur_assignment[rand_idx] == 0) execute_critical_move_no_cons(rand_idx,1);
            // else execute_critical_move_no_cons(rand_idx,-1);
            Float value = (_cur_assignment[rand_idx] == 0) ? 1 : -1;
            if (check_var_shift_bool(rand_idx, value, 1)) execute_critical_move_no_cons(rand_idx, value);
        }
    }

    void qp_solver::update_weight_no_cons()
    {
        // Float obj_delta;
        // for (int mono_pos = 0; mono_pos < _object_monoials.size(); mono_pos++)
        // {
        //     obj_delta = pro_mono(_object_monoials[mono_pos]);
        //     if (_object_monoials[mono_pos].coeff > 0 && obj_delta > 0 && _object_weights[mono_pos] < 10) 
        //     {
        //         if (_object_monoials[mono_pos].m_vars.size() == 1) _object_weights[mono_pos]++;
        //         else if (_cur_assignment[_object_monoials[mono_pos].m_vars[0]] == 1 && _cur_assignment[_object_monoials[mono_pos].m_vars[1]] == 1)
        //             _object_weights[mono_pos]++;
        //     }
        //     else if (_object_monoials[mono_pos].coeff < 0 && obj_delta == 0 && _object_weights[mono_pos] < 10) 
        //     {
        //         if (_object_monoials[mono_pos].m_vars.size() == 1) _object_weights[mono_pos]++;
        //         else if (_cur_assignment[_object_monoials[mono_pos].m_vars[0]] == 0 && _cur_assignment[_object_monoials[mono_pos].m_vars[1]] == 0)
        //             _object_weights[mono_pos]++;
        //     }
        // }
    }

    bool qp_solver::initialize_qubo_gain_cache()
    {
        qubo_gain_cache_active = false;
        if (!qubo_incremental_gain_enabled
            || _object_weights.size() != _object_monoials.size()) return false;

        for (const monomial& mono : _object_monoials)
        {
            if (mono.m_vars.empty() || mono.m_vars.size() > 2) return false;
            for (int v : mono.m_vars)
            {
                if (v < 0 || v >= (int)_vars.size() || !_vars[v].is_bin) return false;
            }
        }
        for (int v : _vars_in_obj)
        {
            if (v < 0 || v >= (int)_vars.size() || !_vars[v].is_bin) return false;
        }

        qubo_gain_cache.assign(_vars.size(), (Float)INT32_MIN);
        qubo_gain_cache_active = true;
        rebuild_qubo_gain_cache();
        return true;
    }

    void qp_solver::rebuild_qubo_gain_cache()
    {
        if (!qubo_gain_cache_active) return;
        bool cache_was_active = qubo_gain_cache_active;
        qubo_gain_cache_active = false;
        std::fill(qubo_gain_cache.begin(), qubo_gain_cache.end(), (Float)INT32_MIN);
        for (int v : _vars_in_obj)
        {
            Float shift = (_cur_assignment[v] == 0) ? 1 : -1;
            qubo_gain_cache[v] = calculate_score_no_cons(v, shift);
        }
        qubo_gain_cache_active = cache_was_active;
        qubo_gain_moves_since_rebuild = 0;
        qubo_gain_cache_rebuild_total++;
    }

    void qp_solver::update_qubo_gain_cache_after_flip(int var_pos, Float change_value)
    {
        if (!qubo_gain_cache_active || var_pos < 0
            || var_pos >= (int)qubo_gain_cache.size()) return;

        Float old_gain = qubo_gain_cache[var_pos];
        if (old_gain == (Float)INT32_MIN)
        {
            rebuild_qubo_gain_cache();
            return;
        }
        qubo_gain_cache[var_pos] = -old_gain;

        for (int mono_pos : _vars[var_pos].obj_monomials)
        {
            const monomial& mono = _object_monoials[mono_pos];
            if (!mono.is_multilinear || mono.m_vars.size() != 2) continue;
            int other = mono.m_vars[0] == var_pos ? mono.m_vars[1] : mono.m_vars[0];
            if (other == var_pos || other < 0 || other >= (int)qubo_gain_cache.size()) continue;
            Float other_gain = qubo_gain_cache[other];
            if (other_gain == (Float)INT32_MIN) continue;
            Float other_shift = (_cur_assignment[other] == 0) ? 1 : -1;
            qubo_gain_cache[other] -= mono.coeff * _object_weights[mono_pos]
                * change_value * other_shift;
            qubo_gain_cache_neighbor_updates_total++;
        }

        qubo_gain_moves_since_rebuild++;
        if (qubo_gain_rebuild_interval > 0
            && qubo_gain_moves_since_rebuild >= qubo_gain_rebuild_interval)
        {
            rebuild_qubo_gain_cache();
        }
        else if (qubo_gain_verify_interval > 0
                 && qubo_gain_moves_since_rebuild % qubo_gain_verify_interval == 0)
        {
            verify_qubo_gain_cache();
        }
    }

    bool qp_solver::verify_qubo_gain_cache(Float tolerance)
    {
        if (!qubo_gain_cache_active) return true;
        bool cache_was_active = qubo_gain_cache_active;
        qubo_gain_cache_active = false;
        bool valid = true;
        for (int v : _vars_in_obj)
        {
            Float shift = (_cur_assignment[v] == 0) ? 1 : -1;
            Float exact = calculate_score_no_cons(v, shift);
            Float cached = qubo_gain_cache[v];
            Float allowed = tolerance * (1.0 + std::abs(exact));
            if (std::abs(exact - cached) > allowed)
            {
                qubo_gain_cache_mismatch_total++;
                valid = false;
                if (qubo_gain_cache_mismatch_total <= 5)
                {
                    std::cerr << "[QUBO_GAIN_CACHE] mismatch var=" << v
                              << ", exact=" << exact << ", cached=" << cached << std::endl;
                }
                break;
            }
        }
        qubo_gain_cache_active = cache_was_active;
        return valid;
    }

    Float qp_solver::calculate_score_no_cons(int var_pos, Float change_value)
    {
        if (qubo_gain_cache_active && var_pos >= 0
            && var_pos < (int)qubo_gain_cache.size())
        {
            Float expected_shift = (_cur_assignment[var_pos] == 0) ? 1 : -1;
            if (std::abs(change_value - expected_shift) <= eb
                && qubo_gain_cache[var_pos] != (Float)INT32_MIN)
            {
                qubo_gain_cache_hits_total++;
                return qubo_gain_cache[var_pos];
            }
        }
        //TODO:
        Float score = 0;
        for (int mono_pos : _vars[var_pos].obj_monomials)
        {
            score -= pro_mono_inc(_object_monoials[mono_pos], var_pos) * _object_weights[mono_pos];
        }
        return score;
    }

    void qp_solver::execute_critical_move_no_cons(int var_pos, Float change_value)
    {
        // change the value of cons, sat state
        // change the is_cur_feasible
        // change the tabu of var
        // change the cur_value
        var * nor_var;
        _cur_assignment[var_pos] += change_value;
        update_qubo_gain_cache_after_flip(var_pos, change_value);
        nor_var = & (_vars[var_pos]);
        if (change_value < 0) _vars[var_pos].last_pos_step = _steps + 3 + rand() % 10;
        else _vars[var_pos].last_neg_step = _steps + 3 + rand() % 10;
        update_best_solution();
    }

    void qp_solver::insert_operation_no_cons()
    {
        polynomial_constraint * unbound_con;
        all_coeff * a_coeff;
        var * obj_var;
        Float delta;
        Float linear_coeff_value = 0;
        Float li_var_coeff;
        int li_var_idx;
        Float cur_value;
        Float change_value;
        _operation_vars_sub.clear();
        _operation_value_sub.clear();
        for (int i = 0; i < _vars.size(); i++)
        {
            if (!qubo_gain_cache_active) _cur_delta[i] = INT32_MIN;//这里也有问题
        }
        for (int var_pos : _vars_in_obj)
        {
            // if (rand() % 10000 < 5000) continue;
            obj_var = & (_vars[var_pos]);
            cur_value = _cur_assignment[var_pos];
            change_value = (cur_value == 0) ? 1 : -1;
            if (qubo_gain_cache_active)
            {
                if (qubo_gain_cache[var_pos] > 0
                    && check_var_shift_bool(var_pos, change_value, false))
                {
                    _operation_vars_sub.push_back(var_pos);
                    _operation_value_sub.push_back(change_value);
                }
                continue;
            }
            //execute the best value of objvar
            linear_coeff_value = obj_var->obj_constant_coeff;
            for (int linear_pos = 0; linear_pos < obj_var->obj_linear_coeff.size(); linear_pos++)
            {
                li_var_idx = obj_var->obj_linear_coeff[linear_pos];
                li_var_coeff = obj_var->obj_linear_constant_coeff[linear_pos];
                linear_coeff_value += _cur_assignment[li_var_idx] * li_var_coeff;
            }
            linear_coeff_value += obj_var->obj_quadratic_coeff;
            if ((linear_coeff_value < 0 && cur_value == 0) || (linear_coeff_value > 0 && cur_value == 1))
            {
                if (check_var_shift_bool(var_pos, change_value, false))
                {
                    _operation_vars_sub.push_back(var_pos);
                    _operation_value_sub.push_back(change_value);
                }
            }
        }
    }

    void qp_solver::select_best_operation_no_cons(int & var_pos, Float & change_value, Float & score)
    {
        score = INT32_MIN;
        int cnt;
        int op_size = _operation_vars_sub.size();
        bool is_bms;
        bms = 200;
        if (op_size <= bms) //200
        {
            cnt = op_size;
            is_bms = false;
        } 
        else 
        {
            cnt = bms;
            is_bms = true;
        }
        int cur_var, best_var;
        Float cur_shift, cur_score;
        int rand_index;
        for (int i = 0; i < cnt; i++) 
        {
            if (is_bms) 
            {
                rand_index = rand() % (op_size - i);
                cur_var = _operation_vars_sub[rand_index];
                cur_shift = _operation_value_sub[rand_index];
                _operation_vars_sub[rand_index] = _operation_vars_sub[op_size - i - 1];
                _operation_value_sub[rand_index] = _operation_value_sub[op_size - i - 1];
            } 
            else {
                cur_var = _operation_vars_sub[i];
                cur_shift = _operation_value_sub[i];
            }
            cur_score = calculate_score_no_cons(cur_var, cur_shift);
            _cur_delta[cur_var] = - cur_score;
            if (cur_score > score) {
                score = cur_score;
                var_pos = cur_var;
                best_var = cur_var;
                change_value = cur_shift;
            }
            else if (cur_score == score)
            {
                int cur_age = std::max(_vars[cur_var].last_pos_step, _vars[cur_var].last_neg_step);
                int best_age = std::max(_vars[best_var].last_pos_step, _vars[best_var].last_neg_step);
                if (cur_age < best_age)
                {
                    score = cur_score;
                    var_pos = cur_var;
                    best_var = cur_var;
                    change_value = cur_shift;
                }
            }
        }
    }

    void qp_solver::initialize_without_cons()
    {
        polynomial_constraint * pcon;
        Float mono_delta = 0, obj_delta = _obj_constant;
        var * bin_var;
        // int var_pos;
        Float con_delta;
        Float var_delta;
        int score = 0;
        int state;
        int coeff_postive;
        bool is_pos;
        int cur_score;
        _var_num = _vars.size();
        _bool_var_num = _bool_vars.size();
        _int_var_num = _int_vars.size();
        //构建LS的信息,类里其它信息的构建,初始值的给出,约束的满足和不满足,初始化二进制变量的score:
        if (problem_type != 0)
        {
            for (int i = 0; i < _var_num; i++)
            {
                if (_vars[i].is_bin) _cur_assignment.push_back(0);
                else if (_vars[i].has_lower) 
                {  
                    if (_int_vars.find(i) != _int_vars.end()) _cur_assignment.push_back(ceil(_vars[i].lower));
                    _cur_assignment.push_back(_vars[i].lower);
                }
                else if (_vars[i].has_upper)
                {
                    if (_int_vars.find(i) != _int_vars.end()) _cur_assignment.push_back(floor(_vars[i].lower));
                    _cur_assignment.push_back(_vars[i].upper);
                }
                else _cur_assignment.push_back(1);
            }
        }
        else 
        {
            for (int i = 0; i < _var_num; i++)
            {
                if (_vars[i].is_bin) _cur_assignment.push_back(0);
                else if (_vars[i].has_lower) _cur_assignment.push_back(ceil(_vars[i].lower));
                else if (_vars[i].has_upper) _cur_assignment.push_back(floor(_vars[i].lower));
                else _cur_assignment.push_back(1);
            }
        }
        is_feasible = true;
        is_cur_feasible = true;
        //TODO:上下界初始化
        _object_weight = 1;
        _best_assignment = _cur_assignment;
        for (int mono_pos = 0; mono_pos < _object_monoials.size(); mono_pos++)
        {
            // cout << pro_mono(_object_monoials[mono_pos]) <<endl;
            obj_delta += pro_mono(_object_monoials[mono_pos]);
            // cout <<" cur obj = "<< obj_delta <<endl;
            _object_weights.push_back(1);
        }
        _best_object_value = obj_delta;
        // cout <<" init value: "<< _best_object_value << endl;
        for (int i = 0; i < _vars.size(); i++)
        {
            _cur_delta.push_back(INT32_MIN);//这里也有问题
        }
        // for (int var_pos : _bool_vars)
        // {
        //     // var_pos = _bool_vars[bin_size];
        //     bin_var = & (_vars[var_pos]);
        //     score = 0;
        //     is_pos = (_cur_assignment[var_pos] > 0);
        //     coeff_postive = pro_var_delta_in_obj(bin_var, var_pos, is_pos);
        //     score += _object_weight * coeff_postive;
        //     bin_var->obj_score = _object_weight * coeff_postive;
        //     bin_var->bool_score = score;
        // }
    }

    void qp_solver::local_search_without_cons()
    {
        std::srand(seed_num);
        cout << " ls_no_cons " << endl; 
        
        // Phase F: Disable block for unconstrained
        if (block_disable_for_unconstrained && _constraints.size() == 0) {
            block_disabled_unconstrained_total++;
            // cout << "block disabled reason = unconstrained" << endl;
        }

        initialize_without_cons();
        initialize_qubo_gain_cache();
        int var_pos;
        Float change_value, score;
        size_t coupled_objective_terms = 0;
        for (const monomial& mono : _object_monoials) {
            if (mono.is_multilinear) coupled_objective_terms++;
        }
        bool diversification_eligible = serial_diversification_enabled
            && !_bool_vars.empty()
            && (Float)coupled_objective_terms / std::max((size_t)1, _vars.size())
                <= serial_diversification_max_density;
        bool long_run_diversification = _cut_off >= serial_diversification_long_run_threshold;
        int max_diversifications = long_run_diversification
            ? serial_diversification_long_max_restarts : 1;
        Float observed_best = _best_object_value;
        double last_improvement_time = 0.0;
        for (_steps = 0; _steps <= _max_steps; _steps++)
        {
            //TODO
            if (_steps % 1000 == 0)
            {
                double elapsed = TimeElapsed();
                if (elapsed > _cut_off) break;
                if (_best_object_value < observed_best - eb)
                {
                    observed_best = _best_object_value;
                    last_improvement_time = elapsed;
                }

                Float next_ratio = long_run_diversification
                    ? serial_diversification_long_first_ratio
                        + serial_diversification_total
                            * serial_diversification_long_interval_ratio
                    : serial_diversification_time_ratio;
                bool sufficiently_stagnant = !long_run_diversification
                    || elapsed - last_improvement_time
                        >= _cut_off * serial_diversification_min_stagnation_ratio;

                // Phase H-2: preserve the incumbent, then start a serial ILS
                // trajectory from a bounded perturbation of that incumbent.
                if (diversification_eligible
                    && serial_diversification_total < max_diversifications
                    && elapsed >= _cut_off * next_ratio
                    && sufficiently_stagnant
                    && !_best_assignment.empty())
                {
                    _cur_assignment = _best_assignment;
                    vector<int> bool_vars(_bool_vars.begin(), _bool_vars.end());
                    int kick_limit = serial_diversification_kick_cap;
                    if (long_run_diversification)
                    {
                        kick_limit += serial_diversification_total
                            * serial_diversification_long_kick_growth;
                    }
                    int kick_count = std::max(2, std::min(kick_limit,
                        (int)std::sqrt((double)bool_vars.size())));
                    std::srand(seed_num + 104729 * (serial_diversification_total + 1));
                    for (int i = 0; i < kick_count && i < (int)bool_vars.size(); ++i)
                    {
                        int pick = i + rand() % (bool_vars.size() - i);
                        std::swap(bool_vars[i], bool_vars[pick]);
                        int v = bool_vars[i];
                        _cur_assignment[v] = (_cur_assignment[v] == 0) ? 1 : 0;
                        _vars[v].last_pos_step = -10;
                        _vars[v].last_neg_step = -10;
                    }
                    rebuild_qubo_gain_cache();
                    serial_diversification_total++;
                    last_improvement_time = elapsed;
                }
            }
            insert_operation_no_cons();//待采样，好像BMS就是采样了，除非增量式
            select_best_operation_no_cons(var_pos, change_value, score);
            //下面的都得改
            if (score > 0) execute_critical_move_no_cons(var_pos, change_value);
            else if (!fps_move())
            {
                update_weight_no_cons();
                random_walk_no_cons();
            }
        }
        if (_best_object_value == INT32_MAX) 
        {
            cout << "process no cons with obj = max error" << endl;
        }
        else
        {
            if (is_minimize)
                cout << std::fixed << " best obj min= " ;
            else 
                cout << std::fixed << " best obj max= " ;
            if (is_minimize) cout << _best_object_value + _obj_constant;
            else cout << -_best_object_value + _obj_constant;
            if (serial_diversification_enabled) {
                std::cerr << "serial_diversification_total = "
                          << serial_diversification_total << std::endl;
            }
            std::cerr << "search_steps = " << _steps << std::endl;
            if (qubo_incremental_gain_enabled) {
                std::cerr << "qubo_gain_cache_active = " << qubo_gain_cache_active << std::endl;
                std::cerr << "qubo_gain_cache_hits_total = " << qubo_gain_cache_hits_total << std::endl;
                std::cerr << "qubo_gain_cache_rebuild_total = " << qubo_gain_cache_rebuild_total << std::endl;
                std::cerr << "qubo_gain_cache_neighbor_updates_total = "
                          << qubo_gain_cache_neighbor_updates_total << std::endl;
                std::cerr << "qubo_gain_cache_mismatch_total = "
                          << qubo_gain_cache_mismatch_total << std::endl;
            }
            // cout << endl << " solution : " << endl;
            // print_best_solution();
        } 
    }

}
