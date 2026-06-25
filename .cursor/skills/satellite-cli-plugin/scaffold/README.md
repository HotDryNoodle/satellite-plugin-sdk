# Scaffold bundle (cli-for-agents satellite plugin)

This directory ships inside the **satellite-cli-plugin** Cursor skill. Copy the whole skill folder to bootstrap plugins in any repo:

```bash
cp -r /path/to/satellite-cli-plugin ~/.cursor/skills/
# or per-project:
cp -r /path/to/satellite-cli-plugin .cursor/skills/
```

Then scaffold into your project:

```bash
.cursor/skills/satellite-cli-plugin/scripts/init_plugin.sh \
  --executable my-plugin \
  --tool-name mydomain.mycapability \
  --domain mydomain \
  --root .
```

## Contents

| Path | Purpose |
|------|---------|
| `src/PLUGIN_NAME/` | C++ CLI entry + business stub (needs `mp_common` in mission-planer) |
| `wrapper/PLUGIN_NAME.py` | Portable Python entry (no C++ build) |
| `configs/plugins/` | Manifest template |
| `schemas/` | Input/output + `plugin_manifest.schema.json` |
| `samples/` | Example request |
| `meson.build.snippet` | Paste into Meson root |

## Reference implementation

In mission-planer repo: `src/mission-planer/` (not generated from this scaffold).
