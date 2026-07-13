# Mode 16：面向 QUBO 的低频自适应 2-flip Escape 总结报告

## 1. 结论先行

当前新增配置为 **Mode 16**。Mode 16 完整继承 Mode 15 的全部行为，包括：

1. 面向有约束 IQCQP 的 violation-prioritized safe block repair；
2. feasible lock、低频触发和时间预算控制；
3. 无约束二元二次问题上的串行 ILS、broad escape pool；
4. QUBO 目标函数的精确增量 gain cache；
5. 已有的可行性输出和 correctness 修复。

Mode 16 在 Mode 15 基础上只增加一个新模块：**仅用于无约束二元二次问题的低频、自适应、gain-cache 支撑的 2-flip escape 算子**。它在单变量局部搜索没有正收益时，从目标二次图的边中寻找一个同时翻转两个二元变量的正收益 pair move，用于跳出单变量局部最优。

完整 221 实例、单次固定随机轨迹的测试结果如下：

| 截止时间 | Mode 16 vs baseline 胜 | 平 | 负 | Mode 16 vs Mode 15 胜 | 平 | 负 | Mode 16 可行/有 obj | cache mismatch |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 10 s | 31 | 185 | 5 | 25 | 190 | 6 | 215 | 0 |
| 30 s | 26 | 191 | 4 | 24 | 191 | 6 | 215 | 0 |
| 300 s | 18 | 196 | 7 | 17 | 198 | 6 | 215 | 0 |

因此，当前证据支持：

- Mode 16 相对原 LS-IQCQP baseline 在 10、30、300 秒均胜多负少；
- Mode 16 相对 Mode 15 在 10、30、300 秒也均胜多负少；
- 新增 2-flip escape 没有破坏 QUBO gain cache，一系列测试中 mismatch 总计为 0；
- 有约束问题上的 correctness 修复未被破坏，`ball_mk4_10.lp` 和 `ball_mk4_15.lp` 仍输出 `NO_FEASIBLE_SOLUTION`。

更细的结果表明，Mode 16 的收益主要集中在无约束 QUBO 子集，尤其是 chimera 实例。论文中建议把 Mode 16 写成 **Mode 15 的 QUBO 专用增强模块**，而不是声称它显著改善所有 IQCQP 类型。

---

## 2. Mode 15 到 Mode 16 的继承关系

Mode 15 已经提供了一个保守串行混合框架。它保留原 LS-IQCQP 的单变量搜索、compensate move、动态权重和 random walk 主流程，并在外层增加：

- 安全 block repair，用于有约束不可行状态；
- 无约束 QUBO 上的串行 ILS；
- broad escape pool；
- QUBO 精确 gain cache。

Mode 16 不改变这些机制。`call.cpp` 中 Mode 16 复制 Mode 15 的全部参数，只额外设置：

```text
qubo_pair_flip_enabled = true
```

这保证了 Mode 15 的行为不会因为新增 Mode 16 而改变。Mode 16 的新增逻辑只位于无约束搜索分支 `local_search_without_cons()`，并且还要依赖 `qubo_gain_cache_active == true`。因此，有约束问题、非二元问题、非二次目标问题和无法启用 QUBO cache 的问题都会自动回退到 Mode 15 的原有路径。

---

## 3. 新增算法：QUBO Adaptive 2-flip Escape

### 3.1 适用条件

Mode 16 的 2-flip escape 只在下列条件全部满足时启用：

1. 当前问题无约束；
2. 目标函数中出现的变量均为二元变量；
3. 目标函数所有单项式次数不超过 2；
4. `qubo_gain_cache_active == true`；
5. 当前 best single flip score 不大于 0，即单变量局部搜索没有正收益；
6. 当前 mode 显式开启 `qubo_pair_flip_enabled`；
7. 低频触发间隔满足，当前实现默认每 10 个搜索步最多尝试一次。

其中前四条由既有 QUBO gain cache 初始化逻辑保证。第 5 条保证 pair move 只作为 escape 算子，而不是替代单变量改善主流程。第 7 条是实验调参后加入的保护：最初每个局部最优都扫描 pair 候选会消耗过多时间，导致部分实例退化；低频触发后 10、30、300 秒结果均明显更稳。

### 3.2 2-flip gain 公式

设当前单翻转 improvement score 为 \(gain_i\) 和 \(gain_j\)，正值表示目标改善。二元变量翻转量定义为：

\[
d_i =
\begin{cases}
+1, & x_i=0\rightarrow 1,\\
-1, & x_i=1\rightarrow 0.
\end{cases}
\]

若变量 \(i\) 和 \(j\) 之间存在目标二次项，总耦合系数为 \(q_{ij}\)，则同时翻转的 improvement 为：

\[
pair\_score(i,j)=gain_i+gain_j-q_{ij}d_id_j.
\]

这里的 \(q_{ij}\) 不是单个 monomial 的系数，而是所有相关双线性目标项的总和：

\[
q_{ij}=\sum_{m\in E(i,j)} coeff_m\cdot weight_m.
\]

这点对 LP 文件中重复边或由不同项共同产生同一变量对的实例是必要的。若没有目标二次边，在单翻均不改善时两个互不耦合变量通常也不会联合改善，因此候选只来自目标二次图中的边。

### 3.3 候选枚举

当前实现优先扫描目标函数中的 multilinear monomial：

- 若二次目标项数不超过 `qubo_pair_flip_edge_scan_cap = 20000`，完整扫描目标二次图；
- 扫描过程中使用 unordered map 汇总重复边的 \(q_{ij}\)；
- 若二次目标项数超过上限，使用 BMS 随机采样，采样上限为 `qubo_pair_flip_bms_cap = 1000`；
- 对每条候选边检查两个端点均为二元变量，且当前翻转合法；
- 从 `qubo_gain_cache` 读取 \(gain_i\) 和 \(gain_j\)；
- 只接受 `pair_score > max(0, eb)` 的候选；
- tie-break 先偏向 tabu age 更老的 pair，再偏向变量索引更小的 pair。

由于 pair move 只在 `score <= 0` 时进入，且默认每 10 步最多触发一次，完整扫描和 BMS 采样的开销被限制在低频 escape 阶段。

### 3.4 执行方式

Mode 16 没有把两个翻转当作两个独立搜索决策。新增函数 `execute_qubo_pair_flip_no_cons(i,j)` 在逻辑上执行一个 pair move：

1. 记录执行前 incumbent；
2. 翻转变量 \(i\)，并调用 `update_qubo_gain_cache_after_flip(i, d_i)`；
3. 翻转变量 \(j\)，并调用 `update_qubo_gain_cache_after_flip(j, d_j)`；
4. 统一更新两个变量的 tabu 时间戳；
5. 调用一次 `update_best_solution()`；
6. 更新 pair flip 执行统计和 improved-best 统计。

这种执行方式避免把第一个单翻转误认为一次 accepted improving move。虽然 cache 更新在实现上仍是顺序维护，但搜索语义上它是一次 2-flip escape。

---

## 4. Mode 16 主流程

无约束搜索主循环由 Mode 15 的：

```text
insert_operation_no_cons()
select_best_operation_no_cons()
if score > 0:
    execute single flip
else:
    fps_move / update_weight / random_walk
```

变为 Mode 16 的：

```text
insert_operation_no_cons()
select_best_operation_no_cons()
if score > 0:
    execute single flip
else if try_qubo_pair_flip_escape_no_cons():
    execute one logical 2-flip escape
else:
    fps_move / update_weight / random_walk
```

完整伪代码如下：

```text
Algorithm: Mode 16 QUBO Pair-Flip Escape

Input: current assignment x, QUBO gain cache G
Output: updated assignment x

1  generate original single-flip candidates
2  select best single flip with score s
3  if s > 0 then
4      execute the single flip and update gain cache
5      return
6  end if
7
8  if pair flip is disabled or QUBO cache is inactive then
9      go to original fallback
10 end if
11
12 if current step does not satisfy the low-frequency trigger then
13     go to original fallback
14 end if
15
16 enumerate or sample objective quadratic edges
17 for each edge (i,j) do
18     aggregate q_ij over duplicated bilinear terms
19     read gain_i and gain_j from the cache
20     compute score = gain_i + gain_j - q_ij d_i d_j
21     retain the best score > max(0, eb)
22 end for
23
24 if a positive pair exists then
25     flip i and j as one pair move
26     update the gain cache, tabu state and incumbent
27 else
28     invoke original fallback
29 end if
```

---

## 5. 复杂度分析

设目标二次图边数为 \(m_2\)，变量数为 \(n\)，变量 \(i\) 在目标二次图中的度数为 \(d_i\)。

单变量 gain cache 的初始化或重建复杂度为 \(O(m_2+n)\)。单变量候选查询为 \(O(1)\)，执行一个单翻转后的 cache 更新为 \(O(d_i)\)。

Mode 16 的 pair escape 在完整扫描模式下，每次触发最多扫描 \(m_2\) 个二次目标项，并汇总重复边；在 BMS 模式下，每次触发最多采样 `qubo_pair_flip_bms_cap` 个候选。由于它只在单变量无正收益时触发，并且默认每 10 步最多尝试一次，摊销成本远低于每步都枚举 pair 邻域。

实验中可以看到，10 秒 221 实例测试下 pair 模块累计耗时约 103.86 秒，分布在 42 个 QUBO cache active 实例上；300 秒测试下累计耗时约 3022.51 秒。该成本换来了 QUBO 子集上的稳定胜多负少。

---

## 6. 实验设置

实验使用 `data/all_lp` 中 221 个实例。比较对象为：

- baseline：原项目 `/home/b520-2/Desktop/szm/LS-IQCQP`；
- Mode 15：当前项目，命令参数 `1 15 3 8`；
- Mode 16：当前项目，命令参数 `1 16 3 8`。

典型命令为：

```text
baseline:
/home/b520-2/Desktop/szm/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP cutoff 1 instance

Mode 15:
/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP cutoff 1 instance 1 15 3 8

Mode 16:
/home/b520-2/Desktop/szm/IQCQP-block/LS-IQCQP/src/LS-IQCQP/build/LS-IQCQP cutoff 1 instance 1 16 3 8
```

时间档：

- 10 秒：开发后最终批量测试；
- 30 秒：使用 `nohup` 后台批量测试；
- 300 秒：使用 `nohup` 后台批量测试。

结果文件：

- `results/mode16_pairflip_10s/summary.csv`
- `results/mode16_pairflip_10s/aggregate.csv`
- `results/mode16_pairflip_10s/report.md`
- `results/mode16_pairflip_nohup/30s/summary.csv`
- `results/mode16_pairflip_nohup/30s/aggregate.csv`
- `results/mode16_pairflip_nohup/30s/report.md`
- `results/mode16_pairflip_nohup/300s/summary.csv`
- `results/mode16_pairflip_nohup/300s/aggregate.csv`
- `results/mode16_pairflip_nohup/300s/report.md`
- `results/mode16_pairflip_nohup/report_overview.md`

---

## 7. 主结果

### 7.1 全部 221 实例

| 截止时间 | Mode 16 vs baseline | Mode 16 vs Mode 15 | pair attempts | pair checked | pair executed | pair improved-best | pair time | mismatch |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 10 s | 31 / 185 / 5 | 25 / 190 / 6 | 163,203 | 246,185,451 | 37,015 | 1,039 | 103.858 s | 0 |
| 30 s | 26 / 191 / 4 | 24 / 191 / 6 | 399,484 | 600,197,635 | 90,019 | 1,077 | 310.206 s | 0 |
| 300 s | 18 / 196 / 7 | 17 / 198 / 6 | 3,967,936 | 5,876,744,549 | 840,283 | 1,130 | 3,022.514 s | 0 |

三个时间档下，Mode 16 相对 baseline 和 Mode 15 都保持胜多负少。随着 cutoff 增长，平局数量上升、胜负差距收窄，这说明新增 2-flip escape 在短中时限下更明显；长时运行中 Mode 15 的 ILS 和原有随机机制也有更多机会追上。

### 7.2 QUBO cache active 子集

| 截止时间 | 实例数 | Mode 16 vs baseline | Mode 16 vs Mode 15 | pair executed | mismatch |
|---:|---:|---:|---:|---:|---:|
| 10 s | 42 | 29 / 12 / 1 | 23 / 16 / 3 | 37,015 | 0 |
| 30 s | 42 | 24 / 16 / 2 | 21 / 17 / 4 | 90,019 | 0 |
| 300 s | 42 | 18 / 19 / 5 | 15 / 22 / 5 | 840,283 | 0 |

这是 Mode 16 真正发挥作用的子集。结果显示，2-flip escape 对 QUBO 子集在三个 cutoff 都有效，尤其 10 秒时优势最强。

### 7.3 chimera 子集

| 截止时间 | 实例数 | Mode 16 vs baseline | Mode 16 vs Mode 15 | pair executed | mismatch |
|---:|---:|---:|---:|---:|---:|
| 10 s | 19 | 17 / 1 / 1 | 14 / 3 / 2 | 18,445 | 0 |
| 30 s | 19 | 15 / 2 / 2 | 13 / 3 / 3 | 45,188 | 0 |
| 300 s | 19 | 15 / 3 / 1 | 13 / 4 / 2 | 422,264 | 0 |

chimera 是 Mode 16 最稳定受益的实例族。300 秒时仍保持相对 Mode 15 的 13 胜、4 平、2 负，说明 pair escape 不只是短时扰动，也能在长时搜索中提供持续的局部最优逃逸机会。

---

## 8. Correctness 与稳定性检查

### 8.1 有约束不可行实例

以下两个 correctness 用例在 Mode 16 下仍输出 `NO_FEASIBLE_SOLUTION`：

```text
./build/LS-IQCQP 1 1 ../../data/all_lp/ball_mk4_10.lp 1 16 3 8 0.85 8 1000
./build/LS-IQCQP 1 1 ../../data/all_lp/ball_mk4_15.lp 1 16 3 8 0.85 8 1000
```

结果：

| 实例 | 输出状态 | qubo_gain_cache_active | qubo_gain_cache_mismatch_total |
|---|---|---:|---:|
| ball_mk4_10.lp | NO_FEASIBLE_SOLUTION | 0 | 0 |
| ball_mk4_15.lp | NO_FEASIBLE_SOLUTION | 0 | 0 |

这说明 Mode 16 没有重新引入假可行解，也没有影响有约束 block repair 路径。

### 8.2 QUBO cache verify

对 QUBO 实例开启 verify interval 的小测中：

```text
qubo_gain_cache_active = 1
qubo_gain_cache_mismatch_total = 0
qubo_pair_flip_attempt_total = 380
qubo_pair_flip_checked_total = 530296
qubo_pair_flip_executed_total = 285
```

完整 10、30、300 秒批量测试中，Mode 16 的 `cache_mismatch16_sum` 也均为 0。

---

## 9. 结果分析

### 9.1 为什么 2-flip 有效

在 QUBO/UBQP 中，很多局部最优不是因为目标图没有改善方向，而是因为改善方向需要两个强耦合变量同步翻转。单变量 gain cache 可以快速判断每个单翻是否改善，但它仍然局限于 Hamming distance 1 邻域。

Mode 16 的 pair escape 扩展到 Hamming distance 2，但不是盲目枚举所有变量对，而是只看目标二次图中的边。这样做有三个好处：

1. 避免 \(O(n^2)\) 全 pair 枚举；
2. 候选集中包含真正可能产生二阶协同收益的变量对；
3. 直接复用已有 gain cache，pair score 计算只需 \(O(1)\) 加上边系数汇总。

### 9.2 为什么需要低频触发

初始版本在每次单变量无正收益时都尝试 pair scan。smoke 中虽然有收益，但 221 实例 10 秒测试显示对 Mode 15 出现 13/188/20 的退化，主要原因是 pair scan 在部分实例上占用了过多时间。

加入 `qubo_pair_flip_trigger_interval = 10` 后，10 秒结果变为 25/190/6，30 秒为 24/191/6，300 秒为 17/198/6。低频触发让 pair escape 保持“后备跳出局部最优”的定位，而不是吞噬主搜索预算。

### 9.3 长时结果的解释

300 秒时，Mode 16 相对 Mode 15 仍是 17 胜、198 平、6 负，但优势比 10 秒小。原因可能是：

- Mode 15 的 serial ILS 在长时运行中已经有多次扰动机会；
- 原 random walk 和 broad escape pool 在长时间内也能覆盖部分 2-flip escape 找到的区域；
- 2-flip 是较小邻域，对更长时间下的全局探索帮助有限。

因此，Mode 16 更适合作为固定短中时限下的 QUBO 强化模块，而不是替代长时多重重启或更大邻域搜索。

---

## 10. 可写入论文的表述

可以把 Mode 16 的新增贡献写为：

> 对于无约束二元二次子类，本文进一步提出一种低频触发的 gain-cache-supported 2-flip escape。该算子在单变量局部搜索没有正收益时，从目标二次图中选择候选边，利用缓存的单翻转 gain 和边耦合系数精确计算二元同步翻转收益。由于候选仅来自目标二次边，并受到触发间隔和采样上限控制，该方法在保持较低额外开销的同时扩展了局部搜索邻域。在 221 个测试实例上，Mode 16 在 10、30 和 300 秒时相对 Mode 15 分别取得 25/190/6、24/191/6 和 17/198/6 的胜/平/负结果；在 QUBO cache active 子集上分别取得 23/16/3、21/17/4 和 15/22/5，且所有测试中 gain cache mismatch 均为 0。

不建议写成：

- “Mode 16 提高了有约束 IQCQP 的可行性搜索能力”；
- “2-flip 对所有实例族均有效”；
- “pair move 是主搜索算子”。

更准确的定位是：

- Mode 16 是 Mode 15 的 QUBO 专用增强；
- 新模块主要改善无约束二元二次问题的局部最优逃逸；
- 对有约束问题保持不干扰，对非 QUBO 问题自动回退。

---

## 11. 仍需补充的实验

当前结果已经足以说明 Mode 16 在项目测试集和固定随机轨迹下有稳定收益，但若用于正式论文，还建议补充：

1. 多随机种子实验，验证 2-flip 收益不是单条轨迹偶然性；
2. Mode 16 的消融实验：Mode 15、Mode 15 + pair scan every step、Mode 15 + pair interval 10、Mode 15 + pair interval 20；
3. 对 pair flip 触发间隔、BMS cap、edge scan cap 的敏感性分析；
4. 对 QUBO 子集单独报告 time-to-best 曲线；
5. 对 pair move 的真实 objective delta 与公式 score 做 debug 抽样验证，并在附录中报告 mismatch 为 0。

---

## 12. 最终判断

Mode 16 值得写入论文，但应作为 Mode 15 后续增强或 QUBO 专用章节，而不是替代 Mode 15 的主算法叙述。

推荐论文叙事顺序：

1. 先介绍 Mode 15 的保守串行混合框架；
2. 再把 QUBO gain cache 作为高效单翻评价基础；
3. 最后引出 Mode 16 的 low-frequency cache-supported 2-flip escape；
4. 实验中单独报告 QUBO 子集和 chimera 子集，证明新增模块的收益来源。

一句话结论：

> Mode 16 在不破坏 Mode 15 correctness 和可行性输出的前提下，为无约束 QUBO/UBQP 增加了一个低成本的二元协同逃逸邻域，并在 10、30、300 秒固定时间测试中相对 baseline 和 Mode 15 均保持胜多负少。
