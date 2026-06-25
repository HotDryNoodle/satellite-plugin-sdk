# Satellite CLI Plugin — Reference

## Portable vs mission-planer C++ path

| Approach | When | Requirements |
|----------|------|----------------|
| **Python wrapper** | New repo, fast prototype | Python 3, copy skill scaffold |
| **C++ + Meson** | mission-planer or fork with `mp_common` | Meson, `include/mp/common/`, nlohmann_json |
| **Gold standard** | Learn patterns | `src/mission-planer/` in mission-planer repo |

C++ scaffold includes `#include "mp/common/json_io.hpp"`. Vendor those headers or develop inside mission-planer.

## Architecture

```
Agent → task-manager → configs/plugins/*.json → {executable} run --input --work-dir
```

## Domain taxonomy

| domain | example tool_name | safety_class |
|--------|-------------------|--------------|
| `mission` | `mission.remote_sensing_access` | `planning_only` |
| `telemetry` | `telemetry.query` | `planning_only` |
| `payload` | `payload.instrument_cmd` | `command_reversible` |
| `health` | `health.fdir_assess` | `planning_only` |

Full table in skill conversation history or mission-planer docs.

## Exit codes

| Code | Meaning |
|------|---------|
| 0 | success |
| 2 | validation |
| 3 | missing dependency |
| 4 | no_result |
| 5 | retryable |
| 64 | usage |

## Skill directory layout

```
satellite-cli-plugin/
├── SKILL.md
├── reference.md
├── scripts/
│   ├── init_plugin.sh      # --root auto-detect
│   └── smoke_plugin.sh
└── scaffold/               # copy source for new plugins
    ├── src/PLUGIN_NAME/
    ├── wrapper/PLUGIN_NAME.py
    ├── configs/plugins/
    ├── schemas/
    ├── samples/
    └── meson.build.snippet
```

## Distributing to other teams

```bash
# Option A: copy skill only
tar -czf satellite-cli-plugin.tgz -C .cursor/skills satellite-cli-plugin
# Unpack into ~/.cursor/skills/ or .cursor/skills/

# Option B: submodule / subtree in their repo
```

Attach skill in Cursor: `@satellite-cli-plugin` or mention "satellite cli plugin".

## Review checklist

- [ ] Non-interactive CLI with Examples in `--help`
- [ ] Honest manifest capabilities
- [ ] `smoke_plugin.sh` passes
- [ ] `tool_name` matches manifest `name`
