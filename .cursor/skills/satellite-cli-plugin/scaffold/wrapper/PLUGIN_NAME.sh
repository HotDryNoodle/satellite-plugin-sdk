#!/usr/bin/env bash
# Optional shell entry — delegates to Python wrapper.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec python3 "$ROOT/templates/plugin-skeleton/wrapper/PLUGIN_NAME.py" "$@"
