#!/usr/bin/env python3
"""Look up findings in a branch-review review-data.json without re-deriving a search each time.

Examples:
  python find_finding.py review-data.json --id F-012
  python find_finding.py review-data.json --search "quat"
  python find_finding.py review-data.json --module entity-tests --status open
  python find_finding.py review-data.json --severity high --status open
  python find_finding.py review-data.json --status open --category correctness
"""
import argparse
import json
import sys


def status_of(finding: dict) -> str:
    if finding.get("resolved"):
        return "resolved"
    if finding.get("rejected"):
        return "rejected"
    return "open"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("data_file", help="Path to review-data.json")
    parser.add_argument("--id", help="Exact finding id, e.g. F-012")
    parser.add_argument("--search", help="Case-insensitive substring match against id/title/detail/suggestion/file")
    parser.add_argument("--module", help="Filter by module key")
    parser.add_argument("--severity", choices=["high", "medium", "low"], help="Filter by severity")
    parser.add_argument("--category", choices=["correctness", "quality", "test-coverage"], help="Filter by category")
    parser.add_argument("--status", choices=["open", "resolved", "rejected"], help="Filter by resolution status")
    parser.add_argument("--brief", action="store_true", help="One line per match instead of full JSON")
    args = parser.parse_args()

    with open(args.data_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    findings = data.get("findings", [])

    if args.id:
        matches = [f for f in findings if f.get("id") == args.id]
        if not matches:
            print(f"No finding with id {args.id}", file=sys.stderr)
            return 1
    else:
        matches = findings
        if args.module:
            matches = [f for f in matches if f.get("module") == args.module]
        if args.severity:
            matches = [f for f in matches if f.get("severity") == args.severity]
        if args.category:
            matches = [f for f in matches if f.get("category") == args.category]
        if args.status:
            matches = [f for f in matches if status_of(f) == args.status]
        if args.search:
            needle = args.search.lower()
            def haystack(f):
                return " ".join(str(f.get(k, "")) for k in ("id", "title", "detail", "suggestion", "file")).lower()
            matches = [f for f in matches if needle in haystack(f)]

    if not matches:
        print("No findings matched.", file=sys.stderr)
        return 1

    if args.brief or len(matches) > 1:
        for f in matches:
            print(f"{f.get('id','?'):8} [{status_of(f):8}] {f.get('severity','?'):6} {f.get('module','?'):28} {f.get('title','')}")
        if not args.brief:
            print(f"\n{len(matches)} match(es). Pass --id or narrow the filter, or add --brief to keep this short form, to see full detail.", file=sys.stderr)
    else:
        print(json.dumps(matches[0], indent=2))

    return 0


if __name__ == "__main__":
    sys.exit(main())
