#!/usr/bin/env bash
# Scaffold a cli-for-agents plugin from satellite-cli-plugin skill (self-contained).
set -euo pipefail

SKILL_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SCAFFOLD="$SKILL_DIR/scaffold"

usage() {
  cat <<EOF
Usage:
  $(basename "$0") --executable NAME --tool-name DOMAIN.capability --domain DOMAIN \\
    [--description TEXT] [--root PROJECT_DIR]

Example:
  $(basename "$0") \\
    --executable telemetry-query \\
    --tool-name telemetry.query \\
    --domain telemetry \\
    --description "Query onboard telemetry" \\
    --root .

Copies scaffold into PROJECT_DIR (auto-detected: meson.build or configs/plugins/).
Skill path: $SKILL_DIR
EOF
}

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

EXECUTABLE=""
TOOL_NAME=""
DOMAIN=""
DESCRIPTION="New satellite CLI plugin"
ROOT=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --executable) EXECUTABLE="$2"; shift 2 ;;
    --tool-name) TOOL_NAME="$2"; shift 2 ;;
    --domain) DOMAIN="$2"; shift 2 ;;
    --description) DESCRIPTION="$2"; shift 2 ;;
    --root) ROOT="$2"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown arg: $1" >&2; usage; exit 1 ;;
  esac
done

if [[ -z "$EXECUTABLE" || -z "$TOOL_NAME" || -z "$DOMAIN" ]]; then
  usage
  exit 1
fi

if [[ -z "$ROOT" ]]; then
  ROOT="$(find_project_root "$PWD")"
fi
ROOT="$(cd "$ROOT" && pwd)"

PLUGIN_UNDERSCORE="${EXECUTABLE//-/_}"
CPP_NS="mp_${PLUGIN_UNDERSCORE}"
DEST_SRC="$ROOT/src/$EXECUTABLE"
DEST_CFG="$ROOT/configs/plugins/$EXECUTABLE.json"
DEST_SCHEMA_IN="$ROOT/schemas/${EXECUTABLE}.input.schema.json"
DEST_SCHEMA_OUT="$ROOT/schemas/${EXECUTABLE}.output.schema.json"
DEST_SAMPLE="$ROOT/samples/${EXECUTABLE}_request.json"

if [[ -e "$DEST_SRC" || -e "$DEST_CFG" ]]; then
  echo "Refusing to overwrite existing plugin paths for $EXECUTABLE" >&2
  exit 1
fi

copy_and_subst() {
  local src="$1" dest="$2"
  mkdir -p "$(dirname "$dest")"
  sed \
    -e "s/PLUGIN_NAME/$EXECUTABLE/g" \
    -e "s/PLUGIN_UNDERSCORE/$PLUGIN_UNDERSCORE/g" \
    -e "s/PLUGIN_CPP_NS/$CPP_NS/g" \
    -e "s/TOOL_NAME/$TOOL_NAME/g" \
    -e "s/DOMAIN/$DOMAIN/g" \
    -e "s/DESCRIPTION/$DESCRIPTION/g" \
    "$src" > "$dest"
}

mkdir -p "$DEST_SRC"
copy_and_subst "$SCAFFOLD/src/PLUGIN_NAME/main.cpp" "$DEST_SRC/main.cpp"
copy_and_subst "$SCAFFOLD/src/PLUGIN_NAME/PLUGIN_NAME.hpp" "$DEST_SRC/$EXECUTABLE.hpp"
copy_and_subst "$SCAFFOLD/src/PLUGIN_NAME/PLUGIN_NAME.cpp" "$DEST_SRC/$EXECUTABLE.cpp"
copy_and_subst "$SCAFFOLD/configs/plugins/PLUGIN_NAME.json" "$DEST_CFG"
copy_and_subst "$SCAFFOLD/schemas/PLUGIN_NAME.input.schema.json" "$DEST_SCHEMA_IN"
copy_and_subst "$SCAFFOLD/schemas/PLUGIN_NAME.output.schema.json" "$DEST_SCHEMA_OUT"
copy_and_subst "$SCAFFOLD/samples/PLUGIN_NAME_request.json" "$DEST_SAMPLE"

MESON_SNIPPET="$ROOT/var/${EXECUTABLE}_meson.snippet"
mkdir -p "$(dirname "$MESON_SNIPPET")"
copy_and_subst "$SCAFFOLD/meson.build.snippet" "$MESON_SNIPPET"

cat <<EOF

Created plugin scaffold in: $ROOT
  src/$EXECUTABLE/
  configs/plugins/$EXECUTABLE.json
  schemas/${EXECUTABLE}.input.schema.json
  schemas/${EXECUTABLE}.output.schema.json
  samples/${EXECUTABLE}_request.json

Next steps:
  1. Paste meson snippet: $MESON_SNIPPET
  2. meson compile -C build   (C++ path needs satellite-plugin-sdk wrap — see skill reference.md)
  3. $SKILL_DIR/scripts/smoke_plugin.sh --root "$ROOT" $EXECUTABLE

EOF
