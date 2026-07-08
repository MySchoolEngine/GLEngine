---
name: benchmarks
description: Use when asked to run the Google Benchmark suite, measure performance, or compare two benchmark runs (before/after an optimization) on this Windows/MSVC project
---

# Benchmarks

## Overview

Microbenchmarks live under `Benchmarks/` and use [Google Benchmark](https://github.com/google/benchmark).
This skill covers three tasks: **building** the suite, **running** it to a named JSON, and
**comparing** two JSON results with statistical output.

Benchmark numbers are only meaningful in **Release / x64** (the optimizer must be on). Never
report Debug numbers.

## 1. Build the benchmark suite

Benchmarks are **not built by default** — the `Benchmarks` project only exists in the solution
if premake was run with `--benchmarks`.

1. Check whether the project is in the solution. If `Benchmarks.vcxproj` is missing (or a build
   errors with "project does not exist"), regenerate with benchmarks enabled:
   ```
   Tools/Premake5/premake5.exe --benchmarks vs2022
   ```
2. Build **Release / x64**, `Benchmarks` target only (msbuild is not on PATH — use the full path):
   ```powershell
   powershell -Command "& 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe' 'G:\GLEngine\Merging.worktrees\master\Engine.sln' /t:Benchmarks /p:Configuration=Release /p:Platform=x64 /m /nologo /verbosity:minimal"
   ```
   (Adjust the worktree path in the `.sln` if you are not in `master`.)

The executable lands at:
```
bin\Release-windows-x86_64\Benchmarks\Benchmarks.exe
```

## 2. Run to a named JSON

Save every run as JSON so it can be compared later. Results go in `Benchmarks\results\`
(gitignored — see below).

1. Ensure the results dir exists and is ignored by git. On first use create
   `Benchmarks\results\.gitignore` containing:
   ```
   *
   !.gitignore
   ```
   This keeps result JSONs out of commits without touching the root `.gitignore`.

2. Run, writing JSON out. Pick a descriptive `<NAME>` (e.g. `before`, `after`, `master-baseline`):
   ```
   bin\Release-windows-x86_64\Benchmarks\Benchmarks.exe --benchmark_out=Benchmarks\results\<NAME>.json --benchmark_out_format=json
   ```

Useful flags:

| Flag | Purpose |
|------|---------|
| `--benchmark_filter=<regex>` | Run only matching benchmarks, e.g. `--benchmark_filter=BM_BVH` |
| `--benchmark_repetitions=N` | Repeat each benchmark N times (e.g. 10). **Required** for statistical significance in the comparison below — a single run gives only a raw delta, no p-value. |
| `--benchmark_report_aggregates_only=true` | With repetitions, print only mean/median/stddev |

For a trustworthy before/after comparison, prefer:
```
...Benchmarks.exe --benchmark_repetitions=10 --benchmark_out=Benchmarks\results\<NAME>.json --benchmark_out_format=json
```

## 3. Compare two JSON results

Use the bundled `vendor\benchmark\tools\compare.py` in `benchmarks` mode. It needs `scipy` and
`numpy`, so this skill uses a **managed venv at `Benchmarks\.venv`** (also gitignored).

1. Create the venv once (reuse it on every later run — skip this step if `Benchmarks\.venv`
   already exists):
   ```powershell
   py -3 -m venv Benchmarks\.venv
   Benchmarks\.venv\Scripts\python.exe -m pip install --quiet scipy numpy
   ```
   Keep it out of git the same self-contained way as the results dir: on first creation write
   `Benchmarks\.venv\.gitignore` containing:
   ```
   *
   ```
   (a single `*` ignores the entire venv, including that file — no root `.gitignore` edit needed).

2. Compare a baseline against a contender (order is `baseline` then `contender`):
   ```powershell
   Benchmarks\.venv\Scripts\python.exe vendor\benchmark\tools\compare.py benchmarks Benchmarks\results\before.json Benchmarks\results\after.json
   ```

Reading the output:
- The `Time` / `CPU` columns show the **relative change** (contender vs baseline). Negative =
  faster, positive = slower.
- Green = improvement, red = regression.
- If both JSONs were produced with `--benchmark_repetitions`, compare.py runs a Mann-Whitney U
  test and prints per-benchmark p-values — treat changes with `p > 0.05` as noise, not signal.

To compare only a subset, append `--benchmark_filter=<regex>` (applies to both inputs).

## Adding a new benchmark

`.cpp` files under `Benchmarks/` are globbed automatically. Add one, include
`<benchmark/benchmark.h>`, then regenerate (`premake5 --benchmarks vs2022`) and rebuild. See
`Benchmarks/readme.md` for existing coverage.
