---
name: satellite-cli-plugin
description: >-
  Self-contained skill to scaffold and implement cli-for-agents plugins for satellite
  embodied intelligence (TaskManager, manifest, validate/run CLI). Includes copy-paste
  scaffold, init_plugin.sh, and smoke_plugin.sh. Use when adding plugins, reviewing
  CLI compliance, or when the user mentions plugin-skeleton, satellite plugin, or
  整星具生智能插件. Copy this skill folder to ~/.cursor/skills/ or .cursor/skills/ to share.
---

# Satellite CLI Plugin

**Self-contained package** — scaffold, scripts, and contracts live in this skill directory. Other developers copy the whole folder and start immediately.

## Copy to your environment

```bash
# Personal (all projects)
cp -r .cursor/skills/satellite-cli-plugin ~/.cursor/skills/

# Or only this repo (already here if you cloned mission-planer)
```

No dependency on `templates/plugin-skeleton/` — everything is under this skill.

## Quick workflow

```
Task Progress:
- [ ] 1. init_plugin.sh → scaffold into project
- [ ] 2. Implement validate_request() + run_plugin()
- [ ] 3. Register meson.build (+ install_data manifest)
- [ ] 4. smoke_plugin.sh
```

### Step 1 — Scaffold

From project root (auto-detected via `meson.build` or `configs/plugins/`):

```bash
.cursor/skills/satellite-cli-plugin/scripts/init_plugin.sh \
  --executable <kebab-name> \
  --tool-name <domain>.<capability> \
  --domain <domain> \
  --description "<one line>" \
  --root .
```

Outputs: `src/<exe>/`, `configs/plugins/`, `schemas/`, `samples/`, `var/<exe>_meson.snippet`.

Scaffold source: [scaffold/](scaffold/).

### Step 2–3 — Implement + build

- C++ stub: `scaffold/src/PLUGIN_NAME/` → links `mp_common` (mission-planer repo)
- **Portable path**: use `scaffold/wrapper/PLUGIN_NAME.py`, set manifest `executable` to script path
- Meson snippet: `scaffold/meson.build.snippet`

Gold standard (mission-planer only): `src/mission-planer/main.cpp`, `configs/plugins/mission-planer.json`.

### Step 4 — Verify

```bash
meson compile -C build
.cursor/skills/satellite-cli-plugin/scripts/smoke_plugin.sh --root . <executable>
```

## CLI contract

| Command | Required flags |
|---------|----------------|
| `manifest` | — |
| `validate` | `--input FILE` or `--stdin` |
| `run` | `--input` or `--stdin`, `--work-dir DIR` |

Global: `--help`, `--version`. stdout = JSON only. Exit codes: see `reference.md`.

## Honest manifest (v1.1)

Template: `scaffold/configs/plugins/PLUGIN_NAME.json`. Schema: `scaffold/schemas/plugin_manifest.schema.json`.

Default `capabilities`: `async: false`, `cancel: "none"`, `dry_run: true` only if implemented.

## TaskManager envelope

```json
{
  "message_type": "task.submit",
  "tool_name": "domain.capability",
  "idempotency_key": "optional",
  "payload": { "dry_run": true, "request_id": "req_001" }
}
```

## Anti-patterns

- Do not use `image-preprocess` as template (minimal placeholder only)
- Do not declare unimplemented async/cancel in manifest

## Resources

- [scaffold/README.md](scaffold/README.md) — bundle layout
- [reference.md](reference.md) — domains, exit codes, portable vs C++ paths
- Scripts: [scripts/init_plugin.sh](scripts/init_plugin.sh), [scripts/smoke_plugin.sh](scripts/smoke_plugin.sh)
