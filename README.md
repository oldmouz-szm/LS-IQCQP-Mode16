# LS-IQCQP Mode 15

This repository contains a serial enhancement of LS-IQCQP for integer
quadratically constrained quadratic programming. Mode 15 combines:

- stagnation-triggered block repair;
- violation-prioritized Top-k constraint selection;
- safe block-repair acceptance;
- feasible-lock behavior after the first feasible solution;
- a density-gated serial iterated local search for unconstrained binary models;
- an incremental QUBO flip-gain cache.

The original single-variable root operator, pair moves, compensate moves,
dynamic weights, and random-walk fallback remain in the search framework.

## Build

```bash
cd src/LS-IQCQP
make
```

The executable is created at `src/LS-IQCQP/build/LS-IQCQP`.

## Run Mode 15

```bash
./src/LS-IQCQP/build/LS-IQCQP \
  <cutoff_seconds> 1 <instance.lp> 1 15 3 8
```

The arguments after the instance enable block search, select Mode 15, set the
maximum block size to 3, and provide the seed argument. Mode 15 restores the
legacy problem-type-specific seed schedule for baseline compatibility.

## Repository layout

- `src/LS-IQCQP/`: current solver implementation.
- `src/LS-IQCQP initial Variants/`: original experimental variants.
- `data/all_lp/`: the 221 QPLIB/MINLPLib instances used by the project.
- `scripts/`: benchmark and result-parsing scripts.
- `results/`: compact reports and comparison tables; raw logs are excluded.
- `docs/mode15_paper_draft_zh.md`: detailed Chinese method and experiment draft.

## Full comparison

The paired 221-instance experiment at 10, 60, and 300 seconds is summarized in
`results/full_221_mode15_vs_original/summary.md`.

| Cutoff | Mode 15 wins | Ties | Losses | Feasible (Mode 15 / Original) |
|---:|---:|---:|---:|---:|
| 10 s | 26 | 184 | 4 | 214 / 214 |
| 60 s | 17 | 191 | 6 | 214 / 214 |
| 300 s | 17 | 194 | 3 | 214 / 214 |

See `LICENSE`, `data/README.md`, and the benchmark-library citations before
redistributing the source or instance data.
