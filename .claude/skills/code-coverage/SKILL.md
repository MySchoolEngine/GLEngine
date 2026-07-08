---
name: code-coverage
description: Use when asked to measure, report, or investigate test code coverage on Windows/MSVC builds - requires OpenCppCoverage and an already-built test binary
---

# Code Coverage

## Overview

On Windows/MSVC, line coverage is measured with OpenCppCoverage against an already-built test `.exe` — it instruments the binary at runtime, no recompilation with special flags needed. This skill does not build anything; the test project must already be compiled (see `verify-build` skill / memory `reference_build.md`).

## Prerequisites

- `OpenCppCoverage.exe` installed: `winget install OpenCppCoverage` (check first with `where OpenCppCoverage.exe`)
- The relevant `*Test.exe` already built for the configuration you want (Debug is more accurate than Release — Release inlining/optimization can merge or drop lines, understating coverage)

## Process

1. Find the test binary: `bin/Debug-windows-x86_64/<Project>Test/<Project>Test.exe`
2. Scope `--sources` to the file(s) under test — without it, OpenCppCoverage instruments the entire binary including gtest/vendor code, which is slow and noisy.
3. Optionally scope `--gtest_filter` to just the relevant test suite(s), for a faster, focused run.
4. Run:

```bash
OpenCppCoverage.exe --sources "Utils\Utils\SSE\Vec3.*" --export_type html:coverage_report -- "bin\Debug-windows-x86_64\UtilsTest\UtilsTest.exe" --gtest_filter="Vec3_*"
```

(Note the `--` before the test binary path — arguments after it are passed straight to the test executable, e.g. `--gtest_filter`.)

5. Read the results:
   - `coverage_report/index.html` → aggregate percentage across all matched source files
   - `coverage_report/Modules/<TestProject>/<File>.html` → per-line coverage for one file (green = covered, red = uncovered)

## Common Mistakes

- **Omitting `--sources`** — reports coverage for every module loaded (ntdll, gtest, RTTR, ...), burying the file you actually care about.
- **Measuring a Release build** — optimized-away or merged lines report as covered/uncovered unreliably; prefer Debug.
- **Forgetting to rebuild** before measuring — OpenCppCoverage reports against whatever the `.exe` currently contains, not your latest source edits.
- **Windows path separators** — `--sources` patterns use backslashes and match against the paths embedded in the PDB, so use `Module\Path\File.*`, not forward slashes.

## Reference

See `Tests/readme.md` → "Code coverage" section for the canonical project example.
