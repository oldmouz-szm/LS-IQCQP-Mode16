# LS-IQCQP Mode 16

This repository contains a serial enhancement of LS-IQCQP for integer
quadratically constrained quadratic programming (IQCQP). Mode 16 keeps all
Mode 15 behavior and adds a low-frequency, gain-cache-supported two-flip escape
operator for unconstrained binary quadratic problems (QUBO/UBQP).

## Main Features

- Conservative safe block repair for constrained IQCQP.
- Violation-prioritized Top-k constraint selection.
- Feasible-lock behavior after the first feasible solution.
- Serial diversification for unconstrained binary quadratic models.
- Exact incremental QUBO single-flip gain cache.
- Mode 16 QUBO two-flip escape over objective quadratic edges.
- Final incumbent validation and correctness-preserving output status.

Mode 16 is intentionally conservative: the new pair-flip operator is enabled
only when the instance is unconstrained, the objective is binary quadratic, the
QUBO gain cache is active, and no positive single-flip move exists.

## Build

```bash
cd src/LS-IQCQP
make
```

The executable is created at:

```text
src/LS-IQCQP/build/LS-IQCQP
```

## Run

Mode 16:

```bash
./src/LS-IQCQP/build/LS-IQCQP \
  <cutoff_seconds> 1 <instance.lp> 1 16 3 8
```

Mode 15:

```bash
./src/LS-IQCQP/build/LS-IQCQP \
  <cutoff_seconds> 1 <instance.lp> 1 15 3 8
```

The arguments after the instance enable block search, select the mode, set the
maximum block size to 3, and provide the seed argument. Mode 16 inherits the
Mode 15 legacy-compatible seed schedule.

## Repository Layout

- `src/LS-IQCQP/`: current solver implementation.
- `src/LS-IQCQP initial Variants/`: original experimental variants.
- `data/all_lp/`: the 221 QPLIB/MINLPLib instances used by the project.
- `docs/mode15_paper_draft_zh.md`: detailed Mode 15 method and experiment draft.
- `docs/mode16_pairflip_paper_draft_zh.md`: Mode 16 technical report and paper-style summary.
- `scripts/run_mode16_pairflip_cutoffs.sh`: batch runner for 30s/300s Mode 16 comparisons.
- `scripts/qubo_baseline_compare.py`: QUBO external-baseline comparison harness.
- `scripts/run_qubo_external_baselines.sh`: batch runner for D-Wave Tabu, Neal SA, and Gurobi on QUBO instances.
- `results/`: compact reports and comparison tables; raw `.out/.err/.log` runtime logs are excluded.

## Mode 16 Results

The 221-instance paired experiments compare Mode 16 with the original
LS-IQCQP baseline and Mode 15.

| Cutoff | Mode16 vs baseline | Mode16 vs Mode15 | Mode16 feasible/obj | QUBO cache mismatch |
|---:|---:|---:|---:|---:|
| 10 s | 31 / 185 / 5 | 25 / 190 / 6 | 215 | 0 |
| 30 s | 26 / 191 / 4 | 24 / 191 / 6 | 215 | 0 |
| 300 s | 18 / 196 / 7 | 17 / 198 / 6 | 215 | 0 |

QUBO cache-active subset:

| Cutoff | Instances | Mode16 vs baseline | Mode16 vs Mode15 | Pair flips executed |
|---:|---:|---:|---:|---:|
| 10 s | 42 | 29 / 12 / 1 | 23 / 16 / 3 | 37,015 |
| 30 s | 42 | 24 / 16 / 2 | 21 / 17 / 4 | 90,019 |
| 300 s | 42 | 18 / 19 / 5 | 15 / 22 / 5 | 840,283 |

Summary files:

- `results/mode16_pairflip_10s/aggregate.csv`
- `results/mode16_pairflip_nohup/30s/aggregate.csv`
- `results/mode16_pairflip_nohup/300s/aggregate.csv`
- `results/mode16_pairflip_nohup/report_overview.md`

## External QUBO Baselines

The repository also includes a harness for comparing the Mode 16 QUBO subset
against several reproducible external baselines:

- D-Wave `TabuSampler`;
- D-Wave `neal` simulated annealing;
- Gurobi MIQP with a time limit.

On the 42 QUBO cache-active instances with a 10-second cutoff:

| External solver | Mode16 wins | Ties | Mode16 losses |
|---|---:|---:|---:|
| D-Wave Tabu | 11 | 12 | 19 |
| Gurobi | 15 | 5 | 22 |
| Neal SA | 3 | 8 | 31 |

This shows that Mode 16 is a useful QUBO-aware enhancement inside LS-IQCQP, but
it should not be claimed as a state-of-the-art standalone QUBO solver.

## Notes

- Raw solver logs are intentionally excluded from version control.
- Some scripts require optional Python packages such as `dimod`,
  `dwave-samplers`, `dwave-neal`, and `gurobipy`.
- See `LICENSE`, `data/README.md`, and benchmark-library citations before
  redistributing source or instance data.
