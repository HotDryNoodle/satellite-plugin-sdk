# satellite-plugin-sdk

Plugin contract runtime for satellite embodied-intelligence CLI plugins.

- `satellite_common` static library (`satellite::` API)
- `mp/common/*` compatibility forward headers (v0.1)
- JSON schemas for manifest and TaskManager envelopes

## Build

```bash
meson subprojects download
meson setup build
meson compile -C build
```

## Consume from another Meson project

Add `subprojects/satellite-plugin-sdk.wrap` pointing at this repository, then:

```meson
sdk = subproject('satellite-plugin-sdk')
satellite_common_dep = sdk.get_variable('satellite_common_dep')
```
