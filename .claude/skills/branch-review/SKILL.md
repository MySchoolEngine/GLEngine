---
name: branch-review
description: Review a git branch's diff against its base branch using parallel Haiku subagents (one per cohesive group of changed files), and publish the findings as an interactive HTML dashboard artifact — filterable by severity/category/module/status, with a "fix before merge" callout list and per-severity resolved/rejected counts. Use this whenever the user asks for a branch or PR review, a findings dashboard, or says things like "review this branch", "what's outstanding on this branch", "go through the diff and find issues" - even if they don't say "branch review" by name. ALSO use this skill's bundled scripts (not ad hoc grep/node/python one-liners) whenever the user references a finding by its F-NNN id on a review this skill produced - "fix F-040", "mark F-012 rejected", "is F-095 resolved yet", "implement F-033 and F-034" - to look it up and update it instead of writing a fresh search script each time.
---

# Branch Review

Review a branch's changes with parallel cheap subagents, publish the results as a browsable
dashboard, and keep that dashboard in sync as findings get fixed, rejected, or reprioritized.

## Overview

The workflow has two phases that feel different:

1. **Initial review** (once per branch): group the changed files into cohesive modules, dispatch
   one Haiku subagent per module in parallel, merge their findings into `review-data.json`, and
   publish `assets/report_template.html` + that JSON as a multi-file Artifact.
2. **Ongoing iteration** (many times, as the user works through findings): the user says "fix
   F-040", "mark F-012 rejected", "did we already fix F-095?" — look it up with
   `scripts/find_finding.py`, investigate/fix the real code, verify it (build + run tests — don't
   trust reasoning about what a fix *should* do), update the finding with
   `scripts/update_finding.py`, validate with `scripts/validate_report.py`, and republish to the
   **same** artifact URL.

The data lives in `review-data.json`, published alongside the HTML page (Artifacts support
`fetch()` of sibling published files with a relative URL). This is the one thing that makes
phase 2 fast: findings are real JSON you can look up and mutate with a script, not a giant
hand-edited JS object literal embedded in the page that needs a fresh parsing script every time
and a syntax re-check before every republish.

## Phase 1: Initial review

### 1. Scope the diff

```bash
git diff --name-only <baseBranch>...<branch> -- . ':!**/*.png' ':!**/*.bmp' ':!data/**'
```

Exclude generated/binary/asset files that were added but never reviewed as "code" — don't let a
pile of new `.obj`/`.png` test fixtures or vendored assets inflate the file count or get
routed to a subagent that has nothing useful to say about them.

### 2. Group files into modules

Group by subsystem/directory, keeping tightly-coupled things (a header + its test file, a
component + the system that updates it) together. Aim for roughly 10-20 files per module — see
"Grouping files into modules" in `references/subagent_prompt.md` for the full reasoning. Name
each module with a short `key` (lowercase-hyphenated, e.g. `entity-core`), a `title`, and a
one-line `subtitle`.

### 3. Dispatch subagents in parallel

Read `references/subagent_prompt.md` and fill in its template once per module. Launch **all**
modules' subagents in a single message (multiple `Agent` tool calls), `subagent_type:
"general-purpose"`, `model: "haiku"`. Each one writes its own JSON file (module metadata +
findings, no ids yet) to a scratch path you give it — don't have it return findings as chat text,
which is unreliable to parse back out for 10-20 findings at once.

If a subagent's output doesn't parse as valid JSON, or is missing fields, re-read its transcript
rather than guessing — Haiku occasionally wraps the JSON in a markdown fence or adds a stray
sentence before it.

### 4. Merge and assign ids

Read every module's scratch JSON, concatenate their `findings` arrays, and assign sequential
`id`s (`F-001`, `F-002`, ...) in the order you merge them — these ids are what the user will
refer to from now on, so pick a stable, final order once (e.g. module order, then file order
within a module) rather than something you'll want to re-sort later. Build the full
`review-data.json`:

```json
{
  "branch": "feature/my-branch",
  "baseBranch": "master",
  "generatedAt": "2026-10-27",
  "methodology": "One paragraph. Mention any tooling caveats (e.g. \"clang-tidy was configured but broken for this diff - checks were applied manually instead\") so the reader knows what rigor to expect.",
  "modules": [ {"key": "...", "title": "...", "subtitle": "...", "files": 15, "agent": "..."} ],
  "findings": [
    {"id": "F-001", "module": "entity-core", "file": "...", "line": 11, "category": "correctness", "severity": "medium", "title": "...", "detail": "...", "suggestion": "...", "resolved": null, "rejected": null}
  ],
  "callouts": [
    {"findingId": "F-014", "text": "One sentence on why this one matters enough to call out.", "where": "World.cpp:108 · confirmed in 3 sections"}
  ]
}
```

`resolved` and `rejected` start `null` on every finding. `callouts` is a short, hand-curated
"fix before merge" list — not every high-severity finding needs to be there, just the handful
most worth surfacing above the fold (e.g. the same root cause independently flagged by more than
one module, or anything that would actually break the build/runtime).

Run `python scripts/validate_report.py review-data.json` and fix anything it flags before
publishing.

### 5. Publish

Copy `assets/report_template.html` to your scratchpad, then publish it as an Artifact with
`review-data.json` passed via the `files` parameter (so it's served alongside the page at a
relative URL the template's `fetch("review-data.json")` can reach). Give the artifact an `icon`
(e.g. `"clipboard"`) on first publish.

## Phase 2: Iterating on findings

This is most of the work over the life of a review — the user comes back repeatedly with "fix
F-X", "mark F-Y resolved/rejected", "check if F-Z is already fixed", "implement F-A and F-B".

### Look it up

```bash
python scripts/find_finding.py review-data.json --id F-040
python scripts/find_finding.py review-data.json --search "quat"
python scripts/find_finding.py review-data.json --module entity-tests --status open
python scripts/find_finding.py review-data.json --severity high --status open --brief
```

Use this instead of writing a fresh grep/parsing script per request — that's the exact pain this
skill exists to remove.

### Fix it, and verify before trusting it

Read the actual code the finding points at — findings (even high-severity ones) are sometimes
already stale by the time you get to them, sometimes describe something subtler than it sounds,
and their own suggested fix is sometimes just wrong. Build and run the relevant tests after
making a change; don't mark something resolved on the strength of reasoning about what the fix
*should* do. A few patterns worth watching for, learned the hard way across real reviews:

- **The suggested fix doesn't actually work.** Write the regression test *first* if you can, then
  apply the suggested fix, then run the test — if it fails, the suggestion was wrong, not the
  test. Fix it properly and say so in the resolution note.
- **"Dead" code someone flagged for deletion turns out to be load-bearing.** Before deleting
  anything a finding calls unused, grep for it across the whole repo (and the relevant test
  suites) yourself — don't take "no remaining references" at face value.
- **Static analyzer / language-server diagnostics that don't match the real compiler.** If the
  toolchain's own config is broken for this diff (stale `compile_commands.json`, clang-tidy
  misconfigured), say so once in `methodology` and verify everything by building with the real
  toolchain instead of chasing broken-tool noise.
- **A fix that touches shared code can break something the finding didn't mention.** After a
  fix, build and run the tests for every project that depends on the changed code, not just the
  one the finding named.

### Mark it

```bash
python scripts/update_finding.py review-data.json --id F-040 --resolve --commit a1b2c3d \
  --note "Fixed guidToEntity.at() throwing on a missing parent guid; added two tests. EntityTest 47/47 pass."

python scripts/update_finding.py review-data.json --id F-114 --resolve --partial \
  --note "Fixed the 4 loops in MainPassTechnique.cpp; SunShadowMapTechnique.cpp's 3 loops still unfixed."

python scripts/update_finding.py review-data.json --id F-118 --reject \
  --reason "Cosmetic whitespace in dead/commented-out code, not worth a separate change."

python scripts/update_finding.py review-data.json --id F-144 --severity high
```

The `--note`/`--reason` text is what shows up in the dashboard, so make it specific: what changed,
what you verified, and the pass/fail counts — not just "fixed" or "not needed". If a fix is still
sitting in the working tree (not committed), pass `--commit none` rather than a hash that doesn't
exist.

If the user corrects a label you applied before you can act on it ("I actually meant F-041" after
you started on F-040), just redirect — don't revert the first one, since the user may still want
it done; ask only if it's genuinely ambiguous which they mean.

### Validate and republish

```bash
python scripts/validate_report.py review-data.json
```

Then republish the artifact to the **same URL** (pass `url`, not a fresh publish) with the
updated `review-data.json` in `files` — the HTML page itself doesn't need to change.

## Common mistakes

- **Treating a finding's suggested fix as ground truth.** It's a proposal from a cheap, fast
  model reading a diff in isolation — verify it compiles and does what it claims before trusting
  it, same as any other suggestion.
- **Skipping `validate_report.py` before republishing.** It catches duplicate ids, orphaned
  callout references, and missing resolution notes — all cheaper to catch before the user
  notices a broken dashboard than after.
- **Re-deriving a search or an edit by hand instead of using the scripts.** If you catch yourself
  writing a one-off script to find or mutate a finding, that's exactly what
  `find_finding.py`/`update_finding.py` are for.
- **Letting the module grouping balloon a single subagent's scope.** If a module has 40+ files,
  split it — a subagent that's skimming instead of reading closely produces worse findings than
  one with a smaller, fully-readable scope.
- **Forgetting `--commit none`** for an uncommitted fix, which would otherwise show a commit hash
  in the dashboard that doesn't exist in the repo.

## Reference

- `references/subagent_prompt.md` — the exact prompt template for review subagents, plus the
  reasoning behind using Haiku and how to size modules.
- `scripts/find_finding.py`, `scripts/update_finding.py`, `scripts/validate_report.py` — run with
  `--help` (or no args, for `validate_report.py`) for the full option list; all three are plain
  Python standard library, no dependencies to install.
- `assets/report_template.html` — the dashboard shell. Generic and project-agnostic; all
  per-review content comes from `review-data.json` at runtime. Edit this file (not a copy) if you
  want to improve the dashboard itself — e.g. change the stat tiles — since every future review
  should benefit, not just the one in front of you.
