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
2. Scope `--sources` to the file(s) under test — without it, OpenCppCoverage instruments the entire binary including gtest/vendor code, which is slow and noisy. Repeat `--sources` once per file/glob for multiple targets.
3. Optionally scope `--gtest_filter` to just the relevant test suite(s), for a faster, focused run.
4. Run with **both** an HTML export (for browsing) and a Cobertura XML export (for a parseable stat) in the same invocation:

```bash
OpenCppCoverage.exe \
  --sources "Utils\Utils\SSE\Vec3.*" \
  --export_type html:coverage_report \
  --export_type cobertura:coverage_report\coverage.xml \
  -- "bin\Debug-windows-x86_64\UtilsTest\UtilsTest.exe" --gtest_filter="Vec3_*"
```

(Note the `--` before the test binary path — arguments after it are passed straight to the test executable, e.g. `--gtest_filter`. If invoking from the Bash tool rather than PowerShell, a bare backslash before a filename can get eaten — write the XML path as `coverage_report/coverage.xml` or `mv` it into place afterward if it lands as a mangled filename.)

5. Parse the Cobertura XML for an exact overall + per-file line-rate percentage — don't scrape the HTML:

```powershell
[xml]$xml = Get-Content coverage_report\coverage.xml
$root = $xml.coverage
"Overall: {0:N1}% ({1}/{2} lines)" -f ([double]$root.'line-rate' * 100), $root.'lines-covered', $root.'lines-valid'
$xml.SelectNodes("//class") | ForEach-Object {
    "{0,6:N1}%  {1}" -f ([double]$_.'line-rate' * 100), $_.filename
}
```

6. Report back **at minimum**: the report file location(s) (`coverage_report\index.html`, `coverage_report\coverage.xml`) and the overall + per-file coverage percentage from step 5. Optionally write this as `coverage_report\summary.txt` for a persistent record.
7. For line-by-line detail on one file: `coverage_report/Modules/<TestProject>/<File>.html` (green = covered, red = uncovered).

## Common Mistakes

- **Omitting `--sources`** — reports coverage for every module loaded (ntdll, gtest, RTTR, ...), burying the file you actually care about.
- **Measuring a Release build** — optimized-away or merged lines report as covered/uncovered unreliably; prefer Debug.
- **Forgetting to rebuild** before measuring — OpenCppCoverage reports against whatever the `.exe` currently contains, not your latest source edits.
- **Windows path separators** — `--sources` patterns use backslashes and match against the paths embedded in the PDB, so use `Module\Path\File.*`, not forward slashes.
- **Scraping the HTML for a percentage** — the Cobertura XML export gives an exact `line-rate` per file and overall; parse that instead of the HTML summary table.

## Reference

See `Tests/readme.md` → "Code coverage" section for the canonical project example.
