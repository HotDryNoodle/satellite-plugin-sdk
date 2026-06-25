#!/usr/bin/env python3
"""Thin Python wrapper template — same CLI contract as C++ plugins."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

TOOL_NAME = "TOOL_NAME"
VERSION = "0.1.0"


def manifest(pretty: bool) -> int:
    path = Path(__file__).resolve().parents[2] / "configs/plugins/PLUGIN_NAME.json"
    data = json.loads(path.read_text(encoding="utf-8"))
    emit(data, pretty)
    return 0


def validate(args: argparse.Namespace) -> int:
    req = read_input(args)
    ok = "request_id" in req
    emit({"ok": ok, "message": "ok" if ok else "request_id required", "details": {}}, args.pretty)
    return 0 if ok else 2


def run(args: argparse.Namespace) -> int:
    if not args.work_dir:
        print("Error: --work-dir is required for run", file=sys.stderr)
        print("  PLUGIN_NAME.py run --input request.json --work-dir /tmp/task", file=sys.stderr)
        return 2
    req = read_input(args)
    work_dir = Path(args.work_dir)
    work_dir.mkdir(parents=True, exist_ok=True)
    out = {
        "task_id": req.get("request_id", "task_stub"),
        "request_id": req.get("request_id", "task_stub"),
        "tool_name": TOOL_NAME,
        "status": "dry_run" if args.dry_run else "succeeded",
        "artifacts": {"work_dir": str(work_dir)},
        "warnings": ["Python wrapper skeleton stub"],
    }
    emit(out, args.pretty)
    return 0


def read_input(args: argparse.Namespace) -> dict:
    if args.stdin:
        return json.load(sys.stdin)
    if args.input:
        return json.loads(Path(args.input).read_text(encoding="utf-8"))
    raise SystemExit("Error: --input or --stdin required")


def emit(data: dict, pretty: bool) -> None:
    print(json.dumps(data, indent=2 if pretty else None, ensure_ascii=False))


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog="PLUGIN_NAME.py")
    p.add_argument("--version", action="version", version=f"PLUGIN_NAME {VERSION}")
    sub = p.add_subparsers(dest="command", required=True)

    for name, handler in [("manifest", manifest), ("validate", validate), ("run", run)]:
        sp = sub.add_parser(name)
        sp.add_argument("--input")
        sp.add_argument("--stdin", action="store_true")
        sp.add_argument("--work-dir")
        sp.add_argument("--dry-run", action="store_true")
        sp.add_argument("--output", choices=["json", "json-pretty"], default="json")
        sp.set_defaults(handler=handler, pretty=False)

    return p


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    args.pretty = getattr(args, "output", "json") == "json-pretty"
    if args.command == "manifest":
        return manifest(args.pretty)
    return args.handler(args)


if __name__ == "__main__":
    raise SystemExit(main())
