#!/usr/bin/env python3
"""Safely mutate one finding in a branch-review review-data.json (real JSON parsing, not a
hand-edited string replace against embedded JS object literals).

Examples:
  # Mark resolved after verifying a fix (build + run the tests first - this script doesn't):
  python update_finding.py review-data.json --id F-040 --resolve \
      --commit a1b2c3d --note "Fixed guidToEntity.at() throwing on a missing parent guid; added two tests. EntityTest 47/47 pass."

  # Resolved but only partially (note should say what's still open):
  python update_finding.py review-data.json --id F-114 --resolve --partial \
      --note "Fixed MainPassTechnique.cpp's 4 loops; SunShadowMapTechnique.cpp's 3 loops still unfixed."

  # Reject with a reason:
  python update_finding.py review-data.json --id F-118 --reject \
      --reason "Cosmetic whitespace in dead/commented-out code, not worth a separate change."

  # Change severity without touching resolution status:
  python update_finding.py review-data.json --id F-144 --severity high

  # Undo a resolve/reject (back to open):
  python update_finding.py review-data.json --id F-040 --reopen
"""
import argparse
import json
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("data_file", help="Path to review-data.json")
    parser.add_argument("--id", required=True, help="Finding id to update, e.g. F-040")

    action = parser.add_mutually_exclusive_group()
    action.add_argument("--resolve", action="store_true", help="Mark resolved")
    action.add_argument("--reject", action="store_true", help="Mark rejected")
    action.add_argument("--reopen", action="store_true", help="Clear resolved/rejected, back to open")

    parser.add_argument("--commit", help="Short commit hash (--resolve). Use the literal string 'none' for an uncommitted working-tree fix.")
    parser.add_argument("--subject", help="Commit subject line (--resolve), optional")
    parser.add_argument("--note", help="What was actually done, and how it was verified (--resolve) - this is what shows in the report, so make it specific: what changed, which tests, pass/fail counts.")
    parser.add_argument("--partial", action="store_true", help="Mark as only partially resolved (--resolve) - say what's still open in --note")
    parser.add_argument("--reason", help="Why this was rejected (--reject) - be specific: 'verified X' beats 'not a priority'")

    parser.add_argument("--severity", choices=["high", "medium", "low"], help="Change severity independent of resolution status")

    args = parser.parse_args()

    with open(args.data_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    findings = data.get("findings", [])
    target = next((f for f in findings if f.get("id") == args.id), None)
    if target is None:
        print(f"No finding with id {args.id} in {args.data_file}", file=sys.stderr)
        return 1

    if args.resolve:
        if not args.note:
            print("--resolve requires --note (what was done and how it was verified)", file=sys.stderr)
            return 1
        target["resolved"] = {
            "commit": None if (args.commit or "").lower() == "none" else args.commit,
            "subject": args.subject,
            "note": args.note,
            "partial": bool(args.partial),
        }
        target["rejected"] = None
    elif args.reject:
        if not args.reason:
            print("--reject requires --reason", file=sys.stderr)
            return 1
        target["rejected"] = {"reason": args.reason}
        target["resolved"] = None
    elif args.reopen:
        target["resolved"] = None
        target["rejected"] = None

    if args.severity:
        target["severity"] = args.severity

    with open(args.data_file, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
        f.write("\n")

    status = "resolved" if target.get("resolved") else "rejected" if target.get("rejected") else "open"
    print(f"{args.id}: {status}, severity={target.get('severity')}")
    print("Run validate_report.py before republishing the artifact.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
