#!/usr/bin/env bash
# Smoke-test a cli-for-agents plugin executable.
set -euo pipefail

SKILL_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SCAFFOLD="$SKILL_DIR/scaffold"

find_project_root() {
  local start="${1:-$PWD}"
  local dir
  dir="$(cd "$start" && pwd)"
  while [[ "$dir" != "/" ]]; do
    if [[ -f "$dir/meson.build" ]] || [[ -d "$dir/configs/plugins" ]]; then
      echo "$dir"
      return 0
    fi
    dir="$(dirname "$dir")"
  done
  echo "$(cd "$start" && pwd)"
}

ROOT=""
BUILD=""
EXE_NAME=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --root) ROOT="$2"; shift 2 ;;
    --build) BUILD="$2"; shift 2 ;;
    -h|--help)
      echo "Usage: $(basename "$0") [--root DIR] [--build DIR] <executable-name>"
      exit 0
      ;;
    -*)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
    *)
      EXE_NAME="$1"
      shift
      ;;
  esac
done

EXE_NAME="${EXE_NAME:-mission-planer}"
ROOT="${ROOT:-$(find_project_root "$PWD")}"
ROOT="$(cd "$ROOT" && pwd)"
BUILD="${BUILD:-$ROOT/build}"
EXE="$BUILD/$EXE_NAME"

if [[ ! -x "$EXE" ]]; then
  echo "Executable not found: $EXE" >&2
  exit 1
fi

WORK="/tmp/mp_smoke_${EXE_NAME}_$$"
mkdir -p "$WORK"

echo "==> $EXE_NAME manifest"
"$EXE" manifest --output json | python3 -c "import json,sys; m=json.load(sys.stdin); assert m.get('name') and m.get('executable')"

echo "==> $EXE_NAME --version"
"$EXE" --version

echo "==> $EXE_NAME validate --help"
"$EXE" validate --help >/dev/null

echo "==> $EXE_NAME run --help"
"$EXE" run --help >/dev/null

SAMPLE=""
for candidate in \
  "$ROOT/samples/${EXE_NAME}_request.json" \
  "$ROOT/samples/remote_sensing_access.json" \
  "$SCAFFOLD/samples/PLUGIN_NAME_request.json"; do
  if [[ -f "$candidate" ]]; then
    SAMPLE="$candidate"
    break
  fi
done

if [[ -n "$SAMPLE" ]]; then
  echo "==> $EXE_NAME validate --input $SAMPLE"
  set +e
  "$EXE" validate --input "$SAMPLE"
  validate_status=$?
  set -e
  if [[ "$validate_status" != "0" && "$validate_status" != "2" ]]; then
    exit "$validate_status"
  fi

  echo "==> $EXE_NAME run --dry-run"
  set +e
  "$EXE" run --input "$SAMPLE" --work-dir "$WORK" --dry-run --output json
  run_status=$?
  set -e
  if [[ "$run_status" != "0" && "$run_status" != "4" ]]; then
    exit "$run_status"
  fi
fi

echo "Plugin smoke passed: $EXE_NAME"
rm -rf "$WORK"
