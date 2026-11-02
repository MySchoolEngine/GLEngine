# Review subagent prompt template

Use one subagent per review module (a cohesive group of changed files — see "Grouping files
into modules" in SKILL.md). Launch all modules' subagents in parallel, in a single message with
multiple `Agent` tool calls, `subagent_type: "general-purpose"`, `model: "haiku"`.

Fill in the placeholders (`{branch}`, `{baseBranch}`, `{moduleKey}`, `{fileList}`,
`{outputPath}`) and send the result as the subagent's prompt:

---

You are reviewing a subset of the diff between `{branch}` and `{baseBranch}` as part of a larger
code review being split across several agents. This is a **read-only review** — do not edit any
files. Propose fixes in your findings; do not apply them.

**Your files** (module `{moduleKey}`):
{fileList}

**For each file above**, read the diff against `{baseBranch}` and the current file content, then
check for:

1. **Correctness** — logic bugs, null/invalid handle dereferences, use-after-free, off-by-one,
   wrong order of operations, race conditions, resource leaks, anything that would misbehave or
   crash at runtime.
2. **Code quality** — apply the spirit of clang-tidy's `bugprone-*`, `performance-*`,
   `modernize-*`, `readability-*`, and `cppcoreguidelines-*` checks by reading the code (don't
   assume a linter ran — it usually hasn't, or is misconfigured for this diff). Flag dead code,
   needless copies, unclear naming, duplicated logic, anything a reviewer would leave a comment
   about.
3. **Test coverage** — for new or changed production code, is there a test? For new or changed
   test code, does the test actually exercise the interesting case (not just the happy path), and
   does its name match what it asserts?

Also flag, as low-severity quality findings: unrelated formatting/whitespace churn in the diff,
and anything that contradicts this project's own documentation or plan (if you find any nearby).

**Severity guide:**
- `high` — will misbehave, crash, or silently corrupt data/state in a reachable path
- `medium` — a real defect or a meaningful test-coverage gap, but lower likelihood or impact
- `low` — style, naming, redundancy, minor coverage gaps, churn

**Output**: write a JSON file to `{outputPath}` with exactly this shape (no markdown fencing, no
commentary outside the file):

```json
{
  "module": {
    "key": "{moduleKey}",
    "title": "Short human title for this module",
    "subtitle": "One line: what this module covers",
    "files": <number of files you reviewed>,
    "agent": "One sentence summarizing your overall assessment of this module's changes."
  },
  "findings": [
    {
      "file": "relative/path/from/repo/root.cpp",
      "line": 42,
      "category": "correctness | quality | test-coverage",
      "severity": "high | medium | low",
      "title": "Short, specific statement of the defect",
      "detail": "What's wrong, why it matters, and the concrete failure scenario if applicable.",
      "suggestion": "A concrete proposed fix, or null if this is purely a 'needs a test' finding with the fix obvious from the title/detail."
    }
  ]
}
```

Do not invent an `id` — the orchestrator assigns stable `F-NNN` ids when merging every module's
output together, so two agents numbering independently would collide.

Be concrete and specific in `title`/`detail` — avoid vague findings like "could be improved"; name
the actual line, the actual failure mode, the actual test that's missing. It's fine to report zero
findings for a file if it's genuinely fine; don't invent problems to have something to say.

---

## Why Haiku, and why split into modules

A fast, cheap model is enough for this because the task per module is narrow and mechanical
(read a diff, check against a short list of concerns) — it doesn't need deep architectural
judgment, just careful reading. Splitting into modules of roughly 10-20 files each keeps each
agent's context small enough to actually read every file closely, rather than skimming a 150-file
diff. Running them in parallel is what makes a large branch review fast instead of linear in the
number of files.

## Grouping files into modules

Group by subsystem/directory first (e.g. all of `Entity/Entity/Serialization/*` together), then
by how tightly related the files are (a header and its corresponding test file belong together).
Keep each group to a size one agent can actually read closely — as a rough guide, 10-20 files, but
let the natural seams in the diff decide over a strict count: a tightly-coupled 25-file subsystem
is better as one module than split arbitrarily in half.
