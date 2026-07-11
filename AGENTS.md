# satellite-plugin-sdk Guidelines

## Structure

- Public API and implementations: `src/satellite/`
- JSON schemas: `schemas/`
- Samples: `samples/`

## Build

```bash
meson subprojects download nlohmann_json
meson setup build
meson compile -C build
```

Contract tests: `scripts/contract-test.sh`

## Coding Style

C++17, `warning_level=2`. Format with repo-root `.clang-format`.

When working inside `satellite-workspace`, see `skills/coding-style-rules/SKILL.md` at the workspace root. Standalone clone: follow the same rules locally.

- **Naming**: PascalCase types; `snake_case` functions, locals, files (`make_error_json`, `result_envelope.cpp`).
- **API docs**: Doxygen `/** ... */` on public `satellite::` APIs.
- **No silent fallback**; explicit units and error shapes.

Details: workspace `skills/coding-style-rules/references/cpp-style-details.md`.
