# satellite-plugin-sdk

Plugin contract runtime for satellite embodied-intelligence CLI plugins.

- `satellite_common` static library (`satellite::` API)
- `mp/common/*` compatibility forward headers (v0.1)
- JSON schemas for manifest and TaskManager envelopes

## Build

SDK only depends on `nlohmann_json` (no self-wrap):

```bash
meson subprojects download nlohmann_json
meson setup build
meson compile -C build
```

## Consume from another Meson project

Copy the example wrap and point it at a pinned SDK commit:

```bash
mkdir -p subprojects
cp path/to/satellite-plugin-sdk/docs/satellite-plugin-sdk.wrap.example \
  subprojects/satellite-plugin-sdk.wrap
# edit url and revision
```

Then in `meson.build`:

```meson
sdk = subproject('satellite-plugin-sdk')
satellite_common_dep = sdk.get_variable('satellite_common_dep')
```

The SDK repository intentionally does **not** ship `subprojects/satellite-plugin-sdk.wrap` — that file belongs in consumer repos only, so `meson subprojects download` never clones the SDK into itself.
