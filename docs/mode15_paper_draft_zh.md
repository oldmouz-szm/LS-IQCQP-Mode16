# 当前 LS-IQCQP 改进版相对原项目的完整技术说明与论文文稿

## 1. 结论先行

当前最优配置为 **Mode 15**。它不是简单地在原 LS-IQCQP 上增加一种 Block Move，而是保留原单变量 root operator、pair move、compensate move、动态权重和 random walk 主框架，在其外部加入三个互补层次：

1. 面向有约束 IQCQP 的低频、停滞触发、安全多变量修复；
2. 面向无约束二元二次问题的串行迭代局部搜索（ILS）与扩展逃逸候选池；
3. 面向 QUBO 目标函数的精确增量 gain cache，以提高单位时间内可执行的局部搜索步数。

完整 221 实例、单次固定随机轨迹的测试结果为：

| 截止时间 | Mode 15 可行 / 原方法可行 | Mode 15 胜 | 平 | Mode 15 负 | 双方均不可行 |
|---:|---:|---:|---:|---:|---:|
| 10 s | 214 / 214 | 26 | 184 | 4 | 7 |
| 60 s | 214 / 214 | 17 | 191 | 6 | 7 |
| 300 s | 214 / 214 | 17 | 194 | 3 | 7 |

因此，现有证据支持“**相同时间内，Mode 15 在解质量上胜多负少**”，但不支持“已经更快找到可行解”：三个时间档中双方可行实例数完全相同。当前优势主要出现在无约束二元二次实例，尤其是 chimera 和部分 QPLIB 实例。

论文可以把创新点组织为“**保守的分层邻域调度 + 违反度优先的安全块修复 + 结构感知的串行 ILS + 精确增量评价**”。但是，Top-k 约束选择和安全 Block Repair 的可行性收益目前没有被完整实验单独证明；在投稿前必须补做多随机种子、严格消融和 time-to-first-feasible 实验。

---

## 2. 原 LS-IQCQP 的搜索框架

设整数二次约束二次规划为

\[
\min/\max\quad f(x),
\]

\[
g_c(x)\ \{\le,=,\ge\}\ b_c,\qquad c\in\mathcal C,
\]

其中变量可以是二元、整数或实数，目标函数和约束函数包含线性项、平方项及双线性项。

原 LS-IQCQP 根据当前解是否可行切换搜索目标：

- 当当前解不可行时，从 violated constraints 中生成以单变量 root operator 为主的修复操作；如果不存在正得分单变量操作，则尝试 compensate move，随后执行权重更新和随机游走。
- 当当前解可行时，使用单变量或已有 pair move 改善目标函数；如果不存在正得分操作，则更新约束权重并随机游走。
- 约束 tabu 用于抑制短期循环，动态权重用于增强长期未满足约束的重要性。

该机制的优点是单步便宜、对连续变量可以直接利用一元二次方程的根；局限是单变量邻域不能同时协调强耦合变量。例如，两个变量必须同步改变才能降低某个二次约束违反度时，任何单变量中间状态都可能更差，原搜索容易在该位置停滞。

当前项目没有删除上述主流程，也没有用外部求解器替换它。Block Repair 仅在原正得分单变量操作失败后进入，因此属于后备邻域，而不是新的主搜索器。

---

## 3. 改进一：结构感知的多变量 Block Move

### 3.1 Block Move 表示

新增操作表示为

\[
B=(V,\Delta, c, s, r),
\]

其中：

- \(V=(v_1,\ldots,v_k)\) 为同时修改的变量集合；
- \(\Delta=(\delta_1,\ldots,\delta_k)\) 为对应增量；
- \(c\) 为产生该候选的目标约束；
- \(s\) 为候选得分；
- \(r\) 标记该候选属于不可行状态修复还是可行状态目标改善。

Mode 15 使用最大块大小 \(k_{\max}=3\)。执行前检查变量索引唯一性、增量非零、上下界、二元域和整数性，避免非法批量修改。

### 3.2 块变量选择

对目标约束 \(c\) 中的每个变量 \(v\)，代码构造结构权重

\[
h_{cv}=|a_{cv}|+|q_{cv}|(1+|x_v|)
+\sum_j |q_{cvj}|+10I(v\in f)+|\mathcal C(v)|,
\]

其中线性系数、平方系数、双线性耦合强度、变量是否出现在目标函数中以及变量参与约束的数量共同反映其影响力。按 \(h_{cv}\) 降序选出高影响变量，并生成：

- 前两个变量组成的二元块；
- 前三个变量组成的三元块；
- 最高权变量与其最强双线性耦合变量组成的二元块。

该选择不穷举约束中的所有变量子集，而是将组合搜索集中到高影响变量和强耦合变量上。

### 3.3 候选值生成

对块中每个变量生成少量结构化候选值：

- 二元变量：翻转到另一个取值；
- 有界变量：上下界；
- 整数或实数变量：当前值附近的 \(\pm1\)；
- 实数变量：附加 \(\pm0.1\)；
- 目标函数关于该变量的一元驻点；
- 目标约束关于该变量的一元二次根；
- 对整数不等式变量，对根使用 floor、ceil 和 round；
- 对具有两个实根的“\(\le\)”约束，加入两根中点。

每个变量最多保留 5 个合法候选值；各变量候选值的笛卡尔积最多枚举 2000 个组合。由此，算法利用解析根和驻点提供方向信息，同时用硬上限控制块邻域成本。

### 3.4 投影评价与批量执行

候选评价阶段只临时改变块变量，计算投影约束违反度和目标增量后立即恢复原赋值，不改变搜索状态。选中候选后一次性更新所有变量，再统一重算约束状态、不可行约束集合、当前可行性、tabu 信息和 incumbent。

这一设计避免把一个协同变化拆成多个可能不可行的中间单变量状态。

---

## 4. 改进二：停滞感知、预算受控的 Block 触发

直接在每一步枚举块候选会吞噬大部分时间，因此当前算法采用保守触发机制。Mode 15 中 Block Repair 必须同时满足：

1. 当前解不可行；
2. 原单变量修复没有正得分操作；
3. 已执行至少 500 个搜索步；
4. 当前步满足 100 步一次的触发间隔；
5. 总违反度连续 500 步没有改进；
6. Block 模块累计耗时占当前总耗时不超过 5%。

总违反度定义为

\[
V(x)=\sum_{c\in\mathcal C}v_c(x),
\]

其中

\[
v_c(x)=
\begin{cases}
|g_c(x)-b_c|,& c\text{ 为等式},\\
\max(0,g_c(x)-b_c),& c\text{ 为 }\le,\\
\max(0,b_c-g_c(x)),& c\text{ 为 }\ge.
\end{cases}
\]

Mode 15 不启用 feasible block improvement。算法一旦首次到达可行解，Block Repair 被永久关闭，后续完全交回原有目标优化邻域。这一机制称为 feasible lock，其作用是避免多变量修复在已经可行后干扰原 LS-IQCQP 的目标搜索轨迹。

如果块候选不存在、得分非正、检查失败或安全过滤拒绝，算法不会退出，而是继续原有 fallback：compensate move、update weight 和 random walk。

---

## 5. 改进三：Violation-Prioritized Safe Block Repair

这是 Mode 13 开始引入、并由 Mode 15 继承的核心安全机制。

### 5.1 加权归一化违反度排序

原始 Block Repair 会遍历全部当前不可行约束。改进算法首先定义归一化违反度

\[
\bar v_c(x)=\frac{v_c(x)}{1+|b_c|+|g_c(x)|},
\]

再定义优先级

\[
p_c=w_c\bar v_c(x),
\]

其中 \(w_c\) 是原 LS-IQCQP 的动态约束权重。算法只对优先级最高的前 \(K=5\) 个不可行约束生成块候选。优先级相同时，先选择 raw violation 更大的约束；仍相同时选择索引较小的约束，从而保证可复现性。

该规则同时利用“相对违反程度”和“搜索历史累积权重”，避免大数值尺度约束仅因绝对量级而垄断候选预算。

### 5.2 安全接受条件

设块 \(B\) 涉及变量集合 \(V_B\)，受影响约束集合为

\[
\mathcal A(B)=\bigcup_{v\in V_B}\mathcal C(v).
\]

算法只扫描 \(\mathcal A(B)\)，而不是所有约束。定义移动前后受影响约束总违反度

\[
V_A^{old}=\sum_{c\in\mathcal A(B)}v_c(x),
\]

\[
V_A^{new}=\sum_{c\in\mathcal A(B)}v_c(x+\Delta_B).
\]

修复块必须满足两个条件：

**条件一：不破坏当前已满足约束**

\[
v_c(x)\le\varepsilon\Rightarrow
v_c(x+\Delta_B)\le\varepsilon,
\qquad \forall c\in\mathcal A(B).
\]

**条件二：受影响总违反度严格下降**

\[
V_A^{old}-V_A^{new}>
\max\{\varepsilon,\rho(1+|V_A^{old}|)\},
\]

其中 \(\varepsilon=10^{-6}\)，\(\rho=10^{-9}\)。

条件一保护已满足约束，条件二防止仅改善目标约束、却对其他不可行约束造成更大伤害的块移动。该过滤在候选打分前执行，在真正批量修改赋值前再次防御性执行，以防候选状态过期或从其他路径进入执行函数。

### 5.3 修复候选得分

Mode 15 使用非归一化修复得分。对产生候选的目标约束 \(c\)，记目标变化为

\[
\Delta f=f(x+\Delta_B)-f(x),
\]

则候选得分可写为

\[
S(B)=1000w_c[v_c(x)-v_c(x+\Delta_B)]
+1000\sum_{j\in\mathcal A(B)\cap\mathcal U,\,j\ne c}
w_j[v_j(x)-v_j(x+\Delta_B)]
-w_f\Delta f,
\]

其中 \(\mathcal U\) 为当前不可行约束集合，\(w_f\) 为目标权重。算法只执行得分为正的最佳候选。

安全条件负责给出硬保证，得分负责在安全候选中平衡主要约束、其他不可行约束和目标变化。

---

## 6. 改进四：结构感知的串行迭代局部搜索

原项目是串行算法，Mode 15 仍保持单个 solver 进程单线程运行。为了改善长时间搜索，算法只在无约束、含二元变量且目标耦合密度较低的问题上启动串行 ILS。

定义双线性目标项数为 \(m_2\)，变量数为 \(n\)。当

\[
\frac{m_2}{\max(1,n)}\le 2.25
\]

时允许扰动。密度门控用于避免在高度稠密 QUBO 中进行成本高、破坏性强的重启。

每次 ILS 扰动执行以下步骤：

1. 保存全局 incumbent，不覆盖历史最优解；
2. 把当前解恢复为 incumbent；
3. 从二元变量中无放回选择少量变量进行翻转；
4. 清除这些变量的短期 tabu 限制；
5. 重建增量 gain cache；
6. 从扰动解继续同一个串行局部搜索过程。

扰动规模为

\[
k_{kick}=\max\left(2,\min\left(k_{cap},\lfloor\sqrt{|B|}\rfloor\right)\right),
\]

其中 \(|B|\) 为二元变量数。短时运行只在 85% 时间点最多扰动一次，基础上限为 8。运行时间不少于 30 秒时，最多在 50%、70% 和 90% 时间点进行三次扰动；后两次的上限分别按 4 递增。长时扰动还要求距离上次 incumbent 改进至少达到总时限的 5%。

扰动随机种子使用原种子和大质数偏移生成，确保不同重启轨迹确定且可复现。

与随机重启相比，该机制从 incumbent 附近开始，扰动幅度有界，并保留全局最优解，属于 intensification 与 diversification 之间的折中。

---

## 7. 改进五：Broad Escape Pool

原实现只在特定候选插入函数成功时，才把目标变量加入 unbounded-constraint escape pool。Mode 15 在相关线性等式或未定界约束结构中放宽该条件：即使当前特殊候选没有成功插入，只要变量属于这些结构，也可以进入逃逸变量池。

其目的不是直接接受劣质移动，而是扩大停滞时可供原 random-walk/balance 逻辑选择的变量范围，降低候选池过窄造成的重复轨迹。该变化只改变候选池覆盖范围，不改变原单变量执行函数。

---

## 8. 改进六：QUBO 精确增量 Gain Cache

### 8.1 适用范围

该缓存只在下列条件全部满足时启用：

- 问题无约束；
- 目标函数所有单项式次数不超过 2；
- 所有出现在目标函数中的变量均为二元变量；
- 目标单项式和目标权重数据结构一致。

不满足条件时自动回退到原始精确打分，不改变其他问题类型的行为。

### 8.2 增量更新

原无约束搜索在评价变量 \(i\) 的翻转 gain 时，需要遍历所有与 \(i\) 相关的目标项。每轮又要检查大量候选变量，因此重复计算明显。

Mode 15 为每个二元变量保存当前翻转 gain。翻转变量 \(i\) 后：

- \(i\) 自身的反向翻转 gain 取原值的相反数；
- 只更新与 \(i\) 共享双线性项的邻居变量 gain；
- 其他变量 gain 保持不变；
- 每 100000 次移动进行一次全量重建，抑制浮点累积误差。

若 \(d_i\) 为变量 \(i\) 在目标二次图中的度数，原每轮全候选评价近似需要扫描全部目标关联项；使用缓存后，候选 gain 查询为 \(O(1)\)，执行一个翻转后的维护为 \(O(d_i)\)。因此算法将大量重复评价转化为局部邻居更新，使同样时间内能够探索更多搜索步。

在完整实验中，缓存对 42 个实例激活。累计 cache hit 数分别为：10 秒档 63,721,293，60 秒档 362,712,806，300 秒档 1,752,813,210。完整运行的 mismatch 计数为 0，但生产测试没有开启周期性逐项验证，因此正式论文还应增加开启 verify interval 的独立正确性实验。

---

## 9. 随机种子兼容与正确性修复

### 9.1 Legacy-compatible seed schedule

原代码在不同搜索分支中使用不同固定种子：无约束分支为 4，纯二元分支为 8，其他未显式 reseed 的分支等价于 C 随机数生成器的默认初始状态。Mode 14/15 根据问题结构恢复相应种子，以避免“仅仅更换随机种子”被误认为算法收益。

Mode 15 的 ILS 扰动在此基础上使用确定性种子偏移，因此基础轨迹与原方法具有可比性，新增轨迹也可复现。

### 9.2 工程正确性改进

当前项目还包含以下可靠性修复，应在论文实现细节中说明，但不宜包装成算法创新：

- 修复初始化和 restart 中独立 `if` 导致一个变量可能被重复 push 到赋值向量的问题；
- 修复 `best_value = INT32_MIN` 被误写在条件表达式中的赋值错误；
- 正确保存常量变量的常量值；
- 检查输入文件打开失败并显式报错；
- 提供变量域、约束可行性和最终 incumbent 的重新验证；
- Block 执行后统一重算当前约束状态，避免增量状态与真实赋值不一致；
- 最终输出区分 `FEASIBLE` 与 `NO_FEASIBLE_SOLUTION`，并输出最大约束违反度；
- 记录 Block 候选、拒绝原因、执行次数、预算消耗、安全过滤和缓存统计。

Mode 15 为降低热路径开销，关闭了每次 improving incumbent 的严格全量验证，但最终输出前仍执行完整可行性验证。

---

## 10. Mode 15 总体算法流程

下面的伪代码可以直接用于论文。

```text
Algorithm: Conservative Serial Hybrid LS-IQCQP (Mode 15)

Input: IQCQP instance P, cutoff T
Output: best validated feasible solution x_best

1  read P and initialize the legacy LS-IQCQP state
2  select the legacy-compatible random seed by problem type
3  if P is an unconstrained binary quadratic problem then
4      initialize exact flip-gain cache
5  end if
6
7  while elapsed_time < T do
8      if the current solution is feasible then
9          generate original single/pair objective moves
10         if a positive-score original move exists then
11             execute it
12         else
13             update weights and invoke the original random walk
14         end if
15     else
16         generate original root-based single-variable repair moves
17         if a positive-score original move exists then
18             execute it
19         else if the conservative block trigger is satisfied then
20             rank violated constraints by weighted normalized violation
21             retain the top K constraints
22             generate structure-aware blocks and candidate values
23             reject candidates that violate a satisfied constraint
24             reject candidates without strict affected-violation decrease
25             execute the best positive-score safe block, if one exists
26         end if
27         if no block was executed then
28             invoke compensate move; otherwise update weights/random walk
29         end if
30     end if
31
32     if an eligible unconstrained binary search has stagnated and
33        reaches its scheduled ILS time point then
34         restore x_best, apply a bounded binary kick, rebuild gain cache
35     end if
36  end while
37  validate x_best and return it
```

---

## 11. 复杂度分析

设 \(u\) 为当前不可行约束数，\(K=\min(u,5)\)，候选块大小为 \(k\le3\)，每个变量候选数上限为 \(L=5\)，单个块组合数上限为 \(E=2000\)，受影响约束数为 \(|\mathcal A(B)|\)。

违反约束选择需要计算优先级并排序，复杂度约为

\[
O(u\log u).
\]

由于只保留前 5 个约束，后续块生成成本受 \(K\) 控制。一个块的组合枚举最多为

\[
O(\min(L^k,E)).
\]

每个候选只投影评价受影响约束，而不扫描全部约束，主要成本近似为

\[
O(|\mathcal A(B)|+|M_f(B)|),
\]

其中 \(|M_f(B)|\) 表示与块变量相关的目标项评价成本。Block 只在停滞、固定间隔和 5% 时间预算下触发，因此其摊销成本被限制。

对于无约束 QUBO，gain cache 初始化或重建为 \(O(m)\)，候选查询为 \(O(1)\)，执行翻转后的维护为 \(O(d_i)\)。这部分主要优化求解速度，而 ILS 主要优化搜索轨迹的多样性。

---

## 12. 完整实验设置与结果分析

### 12.1 实验设置

- 实例：项目 `data/all_lp` 中全部 221 个实例，来源说明为 QPLIB 和 MINLPLib；其中 137 个文件名为 `QPLIB_*`，其余 84 个属于 chimera、ball、graphpart、nvs 等命名族。
- 编译：双方均使用各自项目中 `-O3` 构建的静态二进制。
- 当前算法：`LS-IQCQP cutoff 1 instance 1 15 3 8`。
- 原算法：`LS-IQCQP cutoff 1 instance`。
- tabu：双方均开启。
- 时间：10、60、300 秒。
- 执行：12 个实例对并发；每个实例对中的当前方法与原方法同时运行，每个 solver 进程单线程。
- 总运行数：\(221\times3\times2=1326\)。
- 异常退出：三个时间档双方均为 0。

### 12.2 主结果

10 秒时，双方均找到 214 个可行解；在双方可行实例中，Mode 15 为 26 胜、184 平、4 负。60 秒为 17 胜、191 平、6 负；300 秒为 17 胜、194 平、3 负。

忽略平局，对每个时间档做双侧 sign test，得到：

- 10 秒：26/30 个非平局实例获胜，\(p\approx5.95\times10^{-5}\)；
- 60 秒：17/23 个非平局实例获胜，\(p\approx0.0347\)；
- 300 秒：17/20 个非平局实例获胜，\(p\approx0.00258\)。

这些结果说明在当前 221 实例和单次确定性轨迹下，胜多负少不太可能由实例级随机对称波动单独解释。不过，实例族之间并非完全独立，而且参数是在相同数据目录上开发的，因此这些 p 值只能作为初步证据，不能替代独立测试集和多随机种子实验。

### 12.3 收益来源

10 秒获胜实例中，15 个来自 19 个 chimera 实例，11 个来自 `QPLIB_*`；300 秒获胜实例中，13 个来自 chimera，4 个来自 `QPLIB_*`。300 秒的 3 个失败实例中，2 个为 chimera，1 个为 QPLIB。

Mode 15 的 gain cache 在 42 个实例上激活；ILS 在其中 21 个低密度实例上激活。10 秒每个合格实例扰动一次，共 21 次；60 秒和 300 秒每个合格实例扰动三次，共 63 次。这与优势主要集中在无约束二元实例的观察一致。

### 12.4 Block Repair 内部统计

在最终 221 实例实验中，Block Repair 实际只在 6 个持续不可行的 ball 实例上触发。300 秒档中：

- 违反约束触发检查总计 1,109,696 次；
- 生成候选 10,778,145 个；
- 执行安全 repair block 1,092,841 次；
- 所有已执行 repair block 均降低了受影响总违反度；
- 安全接受检查 11,870,986 次；
- 拒绝 2,827,846 次，占 23.82%；
- 其中 1,424,705 次阻止了受影响总违反度实际增加，占全部检查的约 12.00%；
- Block 计算总耗时约 61.41 秒，分布在 6 个各运行 300 秒的实例上，约占这些实例总 CPU 时间的 3.41%，低于 5% 预算；
- repair success（一步后得到全局可行解）为 0；
- Top-k 跳过约束数为 0，因为实际触发状态中的不可行约束数没有超过 5。

这组统计证明安全过滤确实大量阻止了无收益或有害候选，并且时间预算有效；但它也表明当前数据没有证明 Top-k 截断带来计算收益，也没有证明安全 Block Repair 提高了最终可行率。论文必须如实区分“机制被执行”和“机制改善最终结果”。

### 12.5 不应使用的聚合方式

三个 cutoff 合计为 60 胜、569 平、13 负，但同一个实例在三个时间档重复出现，不能把 642 个可比较结果当作相互独立样本做显著性检验。论文主表应按 cutoff 分别报告。

---

## 13. 可直接放入论文的摘要草稿

> 本文提出一种面向整数二次约束二次规划的保守串行混合局部搜索算法。该算法保留 LS-IQCQP 的单变量解析根修复、pair move、动态权重和随机游走框架，仅在单变量可行性修复停滞时激活多变量块邻域。为限制块搜索开销并避免破坏已有搜索结构，本文提出加权归一化违反度驱动的 Top-k 约束选择，以及基于受影响约束集合的安全接受准则：候选块不得使已满足约束变为违反，并且必须使受影响总违反度严格下降。对于无约束二元二次子类，本文进一步引入密度门控的串行迭代局部搜索和精确增量翻转收益缓存，在保持单线程执行的同时增加搜索轨迹多样性和单位时间搜索步数。在来自 QPLIB 和 MINLPLib 的 221 个项目实例上，算法在 10、60 和 300 秒时分别取得 26/184/4、17/191/6 和 17/194/3 的胜/平/负结果；双方三个时间档均在 214 个实例上找到可行解，且均无异常退出。结果表明，所提出方法在当前测试集上显著改善了解质量，尤其适用于无约束二元二次实例；其可行性加速能力仍需在更困难的有约束实例和多随机种子设置下进一步验证。

---

## 14. 可直接放入论文的贡献列表

> 本文的主要贡献如下。
>
> 1. 提出一种与原 LS-IQCQP 兼容的分层邻域调度框架。多变量邻域只在单变量修复停滞时触发，并受到最小步数、触发间隔和时间占比的联合控制；达到可行解后立即锁定并恢复原目标搜索。
> 2. 提出 Violation-Prioritized Safe Block Repair。该机制使用动态权重与尺度归一化违反度选择关键约束，并以“不产生新违反约束”和“受影响总违反度严格下降”为硬接受条件，从而抑制多变量修复的副作用。
> 3. 面向无约束二元二次子类，提出密度门控的串行 ILS。算法从 incumbent 附近进行有界扰动，在不使用并行计算的前提下增强长时间搜索的多样性。
> 4. 实现适用于二元二次目标的精确增量 gain cache，将大规模重复候选评价转化为局部邻接更新，并对不满足适用条件的问题自动回退到原评价过程。
> 5. 在 221 个项目实例、三个时间限制下完成与原 LS-IQCQP 的成对实验，验证了改进算法在解质量上的胜多负少，并通过内部统计量分析安全过滤、ILS 和缓存的实际激活范围。

需要注意，第 2 点目前主要由机制统计支持，不应写成“显著提高可行率”；第 5 点应写“项目收集的 221 个实例”，不要写成“完整 QPLIB/MINLPLib”，因为公共库本身远大于 221 个实例。

---

## 15. 当前工作能否支撑论文创新

从方法设计看，已经具备一篇算法改进论文的基本结构，但创新强度来自组合与问题定制，而不是某个从未出现过的通用元启发式概念：

- Top-k、ILS、增量 delta evaluation 分别都是已有思想；
- 更有价值的部分是它们如何围绕 LS-IQCQP 的“不可行优先、可行后优化”状态机组合；
- 安全 Block Repair 的局部不变量、feasible lock 和时间预算，使多变量邻域成为低风险后备算子；
- QUBO cache 与 density-gated ILS 把短时速度和长时跳出局部最优统一到一个串行框架中。

如果目标是一般期刊或会议论文，目前可以形成完整稿件框架；如果目标是较强的优化或人工智能算法期刊，仅有当前单种子主实验还不够。审稿人最可能提出的问题是：

1. 改善来自 ILS、缓存、seed schedule 还是 Block Repair？
2. 为什么可行实例数完全没有改善？
3. Top-k 在主实验中一次都没有真正截断，如何证明其必要性？
4. 只运行一个确定性轨迹，随机算法结论是否稳定？
5. 24 进程并发但没有 CPU pinning，时间比较是否受资源竞争影响？
6. 参数是否在同一 221 实例上调优，是否存在测试集泄漏？

这些问题必须通过下一节的实验补齐。

---

## 16. 投稿前必须补做的实验

### 16.1 干净的组件消融

建议增加独立实验开关，至少比较：

| 配置 | Block | Top-k | Safe filter | Broad pool | ILS | Gain cache |
|---|---:|---:|---:|---:|---:|---:|
| A 原 LS-IQCQP | 0 | 0 | 0 | 0 | 0 | 0 |
| B 种子兼容/正确性修复版 | 0 | 0 | 0 | 0 | 0 | 0 |
| C B + gain cache | 0 | 0 | 0 | 0 | 0 | 1 |
| D C + broad pool | 0 | 0 | 0 | 1 | 0 | 1 |
| E D + ILS | 0 | 0 | 0 | 1 | 1 | 1 |
| F Mode 12 | 1 | 0 | 0 | 0 | 0 | 0 |
| G F + Top-k | 1 | 1 | 0 | 0 | 0 | 0 |
| H F + safe filter | 1 | 0 | 1 | 0 | 0 | 0 |
| I Mode 13 | 1 | 1 | 1 | 0 | 0 | 0 |
| J Mode 15 | 1 | 1 | 1 | 1 | 1 | 1 |

当前 Mode 0--15 不能完全形成上述正交消融，建议只增加实验配置，不修改最终 Mode 15。

### 16.2 多随机种子

每个随机算法至少运行 5 个种子，最好 10 个。对每个实例报告：

- 可行成功率；
- time-to-first-feasible 的中位数和 PAR-2；
- 最终目标的均值、中位数和标准差；
- 相对最佳已知值的 gap；
- anytime 曲线或 primal integral。

原项目内部硬编码种子，需要先把原方法也改成可控 seed 的“只改实验接口、不改算法”版本，否则无法进行对称的多种子比较。

### 16.3 可行性专项实例

当前 221 实例中有 214 个双方均可行，剩余 7 个双方均不可行，无法区分可行性修复能力。应补充一组原方法在 10--300 秒内可行成功率处于 10%--90% 的困难有约束实例，记录首次可行时间，而不仅是最终目标值。

### 16.4 公平计时

正式实验应：

- 固定 CPU 型号、编译器和编译选项；
- 使用单进程串行运行或给每个进程绑定独立物理核；
- 避免超线程兄弟核互相干扰；
- 分离初始化时间与搜索时间，或明确 cutoff 包含哪些阶段；
- 随机化实例和算法运行顺序；
- 保存完整命令、commit/hash、日志和机器负载。

### 16.5 统计检验

建议按实例聚合多个种子后，再做配对 Wilcoxon signed-rank test 或 sign test，并报告 effect size。对多种配置同时比较时进行 Holm 校正。不要把同一实例的不同 cutoff 当作独立样本。

---

## 17. 代码对应关系

核心修改及函数如下：

- `sol.h`
  - `block_move` 数据结构；
  - Block、Phase F/G/H、ILS 和 QUBO cache 参数与统计字段；
  - 新增函数声明。
- `ls_block.cpp`
  - `eval_constraint_value`、`validate_assignment`、`recompute_current_solution_state`；
  - `should_try_block_repair`、`should_try_block_improve`；
  - `normalized_constraint_violation`、`select_repair_constraints_topk`；
  - `safe_accept_block_repair`；
  - `select_candidate_blocks_from_constraint`；
  - `generate_candidate_shifts_for_var`、`generate_candidate_values_for_block`；
  - `calculate_score_block_mix`、`insert_block_operations_mix`；
  - `execute_block_move`、`print_block_statistics`。
- `ls_balance.cpp`
  - 在原可行/不可行状态机的 fallback 位置接入 Block；
  - 维护停滞统计、首次可行锁和 Block 时间预算；
  - broad escape pool；
  - 最终可行性验证与状态输出。
- `ls_no_cons.cpp`
  - QUBO gain cache 初始化、重建、邻接更新和可选验证；
  - 密度门控、时间调度和渐增强 kick 的串行 ILS。
- `call.cpp`
  - Mode 0--15 配置；
  - Mode 15 参数、seed schedule 及可选 ILS/cache 参数入口。
- `component.cpp`、`ls_mix_not_dis.cpp`、`ls_read.cpp`、`ls_bin.cpp`
  - incumbent 验证、初始化、常量读取、文件检查和 seed 接口等正确性修复。

### 17.1 当前项目中的模式演化

Mode 0--15 记录了从积极 Block 到保守串行混合算法的演化过程：

| Mode | 配置含义 |
|---:|---|
| 0 | 完整 Block：repair 和 feasible improvement 均开启，不使用自适应触发 |
| 1 | 仅 Block Repair，不使用自适应触发 |
| 2 | 仅 feasible Block Improvement，不使用自适应触发 |
| 3 | 智能候选 + 原始非归一化得分，不使用自适应触发 |
| 4 | 固定候选 + 归一化得分，不使用自适应触发 |
| 5 | 固定候选 + 原始非归一化得分，不使用自适应触发 |
| 6 | 自适应 Block Repair，使用归一化得分 |
| 7 | 自适应 Block Repair + feasible improvement |
| 8 | 自适应 Block Repair，使用原始非归一化得分 |
| 9 | Legacy-safe，完全关闭 Block |
| 10 | 保守 Repair：500 步预热、500 步停滞、100 步间隔、5% 预算 |
| 11 | 超保守 Repair：1000 步预热、1000 步停滞、200 步间隔、3% 预算 |
| 12 | Mode 10 + 首次可行后永久关闭 Block Repair |
| 13 | Mode 12 + Top-5 违反约束优先级 + Safe Acceptance |
| 14 | Mode 13 + 原项目兼容 seed schedule |
| 15 | Mode 14 + broad escape pool + 串行 ILS + QUBO gain cache |

最终论文应以 Mode 15 为主算法，把 Mode 12、13、14 作为自然消融点；Mode 0--8 更适合说明设计演化，不适合作为最终推荐配置。

---

## 18. 基准库引用

QPLIB 建议引用：

> F. Furini, E. Traversi, P. Belotti, et al. QPLIB: A Library of Quadratic Programming Instances. Mathematical Programming Computation, 2018. DOI: 10.1007/s12532-018-0147-4.

MINLPLib 建议引用：

> M. R. Bussieck, A. S. Drud, and A. Meeraus. MINLPLib—A Collection of Test Models for Mixed-Integer Nonlinear Programming. INFORMS Journal on Computing, 15(1):114–119, 2003.

迭代局部搜索部分应补引 ILS 的经典文献，并在最终参考文献中核对具体版次和页码。LS-IQCQP 原论文的正式作者、题名和出处未在当前源码 README 中给出，提交前必须从原论文补齐，不能只引用代码目录。

---

## 19. 推荐论文题目

中文：

> 面向整数二次约束二次规划的违反度优先安全块修复与串行混合局部搜索

英文：

> Violation-Prioritized Safe Block Repair and Serial Hybrid Local Search for Integer Quadratically Constrained Quadratic Programming
