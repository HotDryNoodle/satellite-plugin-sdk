#!/usr/bin/env bash
# SDK contract test: build + schema validation smoke.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${BUILD:-$ROOT/build}"

if [[ ! -f "$BUILD/meson-private/coredata.dat" ]]; then
  (cd "$ROOT" && meson subprojects download nlohmann_json && meson setup build)
fi
meson compile -C "$BUILD"

python3 - "$ROOT" <<'PY'
import json, pathlib, sys
root = pathlib.Path(sys.argv[1])
schemas = root / 'schemas'
sample = {
    "schema_version": "1.0",
    "message_type": "task.submit",
    "request_id": "req_1",
    "tool_name": "mission.remote_sensing_access",
    "payload": {},
}
schema = json.loads((schemas / 'task_envelope.schema.json').read_text())
required = schema.get('required', [])
for key in required:
    if key not in sample:
        raise SystemExit(f'missing required field in smoke sample: {key}')
print('schema smoke ok')
PY

echo "satellite-plugin-sdk contract test passed"
