#!/usr/bin/env python3
"""Sanity-check a review-data.json before (re)publishing the report artifact.

This replaces re-deriving the same ad hoc integrity checks (duplicate ids, orphaned callout
references, severity counts) by hand every time the data changes. Run it after every
find_finding.py/update_finding.py edit and before every Artifact publish; it exits non-zero on
any problem so it's safe to chain with `&&`.
"""
import json
import re
import sys

ID_PATTERN = re.compile(r"^F-\d{3,}$")
VALID_SEVERITIES = {"high", "medium", "low"}
VALID_CATEGORIES = {"correctness", "quality", "test-coverage"}
REQUIRED_FINDING_FIELDS = ("id", "module", "file", "category", "severity", "title", "detail")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: validate_report.py <review-data.json>", file=sys.stderr)
        return 2

    with open(sys.argv[1], "r", encoding="utf-8") as f:
        data = json.load(f)

    problems = []

    modules = data.get("modules", [])
    findings = data.get("findings", [])
    callouts = data.get("callouts", [])
    module_keys = {m.get("key") for m in modules}
    finding_ids = [f.get("id") for f in findings]

    if not modules:
        problems.append("modules[] is empty")
    if not findings:
        problems.append("findings[] is empty")

    seen_ids = set()
    for i, f in enumerate(findings):
        where = f"findings[{i}] ({f.get('id', '?')})"
        for field in REQUIRED_FINDING_FIELDS:
            if not f.get(field):
                problems.append(f"{where}: missing required field '{field}'")
        fid = f.get("id")
        if fid:
            if not ID_PATTERN.match(fid):
                problems.append(f"{where}: id '{fid}' doesn't match F-NNN")
            if fid in seen_ids:
                problems.append(f"{where}: duplicate id '{fid}'")
            seen_ids.add(fid)
        if f.get("module") and f["module"] not in module_keys:
            problems.append(f"{where}: module '{f['module']}' not in modules[]")
        if f.get("severity") not in VALID_SEVERITIES:
            problems.append(f"{where}: severity '{f.get('severity')}' invalid")
        if f.get("category") not in VALID_CATEGORIES:
            problems.append(f"{where}: category '{f.get('category')}' invalid")
        if f.get("resolved") and f.get("rejected"):
            problems.append(f"{where}: both resolved and rejected are set - pick one (or neither)")
        if f.get("resolved") and not f["resolved"].get("note"):
            problems.append(f"{where}: resolved with no note - the report needs to say what was done")
        if f.get("rejected") and not f["rejected"].get("reason"):
            problems.append(f"{where}: rejected with no reason")

    for i, c in enumerate(callouts):
        target = c.get("findingId")
        if not target:
            problems.append(f"callouts[{i}]: missing findingId")
        elif target not in finding_ids:
            problems.append(f"callouts[{i}]: findingId '{target}' has no matching finding")

    if problems:
        print(f"{len(problems)} problem(s) found:", file=sys.stderr)
        for p in problems:
            print(f"  - {p}", file=sys.stderr)
        return 1

    counts = {"high": 0, "medium": 0, "low": 0}
    resolved = {"high": 0, "medium": 0, "low": 0}
    rejected = {"high": 0, "medium": 0, "low": 0}
    for f in findings:
        counts[f["severity"]] += 1
        if f.get("resolved"):
            resolved[f["severity"]] += 1
        if f.get("rejected"):
            rejected[f["severity"]] += 1

    print(f"OK: {len(modules)} modules, {len(findings)} findings, {len(callouts)} callouts.")
    for sev in ("high", "medium", "low"):
        print(f"  {sev:6}: {counts[sev]:3} total, {resolved[sev]:3} resolved, {rejected[sev]:3} rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
