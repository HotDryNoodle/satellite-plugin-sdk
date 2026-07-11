#!/usr/bin/env bash
# SDK contract test: build + schema validation for active 1.1 samples,
# archived 1.0 fixtures, and canonical types.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${BUILD:-$ROOT/build}"

if [[ ! -f "$BUILD/meson-private/coredata.dat" ]]; then
  (cd "$ROOT" && meson subprojects download nlohmann_json && meson setup build)
fi
meson compile -C "$BUILD"

python3 - "$ROOT" <<'PY'
import json
import pathlib
import sys

from jsonschema import Draft202012Validator
from referencing import Registry, Resource
from referencing.jsonschema import DRAFT202012

root = pathlib.Path(sys.argv[1])
schemas_dir = root / "schemas"
samples_dir = root / "samples"
registry_doc = json.loads((schemas_dir / "schema_registry.json").read_text())
manifest = json.loads((samples_dir / "manifest.json").read_text())

resources = []
for path in schemas_dir.rglob("*.json"):
    if path.name == "schema_registry.json":
        continue
    doc = json.loads(path.read_text())
    uri = doc.get("$id") or path.as_uri()
    resources.append((uri, Resource.from_contents(doc, default_specification=DRAFT202012)))
    # Also register by relative path for local $ref resolution.
    rel = path.relative_to(schemas_dir).as_posix()
    resources.append((rel, Resource.from_contents(doc, default_specification=DRAFT202012)))

ref_registry = Registry().with_resources(resources)


def resolve_schema_path(version: str, kind: str, schema_source: str = "active") -> pathlib.Path:
    if schema_source == "archive":
        table = registry_doc.get("archive", {})
        label = "archive"
    elif schema_source == "active":
        table = registry_doc.get("versions", {})
        label = "versions"
    else:
        raise SystemExit(f"unknown schema_source={schema_source!r}")
    if version not in table:
        raise SystemExit(f"schema_registry missing {label} version {version}")
    entry = table[version]
    if kind not in entry:
        raise SystemExit(f"schema_registry {label} version {version} missing kind {kind}")
    return schemas_dir / entry[kind]


def resolve_canonical_schema_path(canonical_type: str) -> pathlib.Path:
    canonical = registry_doc.get("canonical", {})
    if canonical_type not in canonical:
        raise SystemExit(f"schema_registry missing canonical type {canonical_type}")
    return schemas_dir / canonical[canonical_type]


def load_schema_for_case(case: dict, instance: dict):
    kind = case["kind"]
    if kind == "canonical":
        ctype = case.get("canonical_type")
        if not ctype:
            raise SystemExit(f"{case['id']}: canonical case missing canonical_type")
        path = resolve_canonical_schema_path(ctype)
        return json.loads(path.read_text()), path

    schema_source = case.get("schema_source", "active")
    version = case.get("force_schema_version") or instance.get("schema_version")
    if not version:
        raise SystemExit(f"{case['id']}: missing schema_version")
    path = resolve_schema_path(version, kind, schema_source)
    return json.loads(path.read_text()), path


def apply_extra_rules(instance: dict, rules: list[str]) -> list[str]:
    errors = []
    for rule in rules:
        if rule == "mission_id_ne_task_id":
            mid = instance.get("mission_id")
            tid = instance.get("task_id")
            if mid is not None and tid is not None and mid == tid:
                errors.append("mission_id must not equal task_id")
        else:
            errors.append(f"unknown extra rule: {rule}")
    return errors


def validate_case(case: dict) -> None:
    sample_path = samples_dir / case["path"]
    instance = json.loads(sample_path.read_text())
    kind = case["kind"]
    schema, schema_path = load_schema_for_case(case, instance)
    validator = Draft202012Validator(schema, registry=ref_registry)
    schema_errors = sorted(validator.iter_errors(instance), key=lambda e: list(e.path))
    extra_errors = apply_extra_rules(instance, case.get("extra_rules", []))

    version = case.get("force_schema_version") or instance.get("schema_version")
    # Always enforce F6 on 1.1 documents that carry both ids (positive path).
    if (
        kind != "canonical"
        and version == "1.1"
        and "mission_id_ne_task_id" not in case.get("extra_rules", [])
        and instance.get("mission_id") is not None
        and instance.get("task_id") is not None
        and instance.get("mission_id") == instance.get("task_id")
    ):
        extra_errors.append("mission_id must not equal task_id")

    ok = (not schema_errors) and (not extra_errors)
    expect = case["expect"]
    if expect == "pass" and not ok:
        details = [e.message for e in schema_errors] + extra_errors
        raise SystemExit(
            f"{case['id']} expected pass against {schema_path.name}, got errors: {details}"
        )
    if expect == "fail" and ok:
        raise SystemExit(
            f"{case['id']} expected fail against {schema_path.name}, but validated"
        )
    status = "pass" if ok else "fail"
    print(f"{case['id']}: {status} (expect={expect}) via {schema_path.name}")


# Registry is routing inventory only — no policy keys allowed.
forbidden_registry_keys = {"policy", "fallback", "negotiation", "runtime"}
if any(k in registry_doc for k in forbidden_registry_keys):
    raise SystemExit("schema_registry.json must not contain runtime policy keys")

if "canonical" not in registry_doc or not isinstance(registry_doc["canonical"], dict):
    raise SystemExit("schema_registry.json missing canonical inventory map")

if "1.0" in registry_doc.get("versions", {}):
    raise SystemExit("schema_registry versions must not list retired 1.0")

for case in manifest["cases"]:
    validate_case(case)

print(f"schema contract ok ({len(manifest['cases'])} cases)")
PY

echo "satellite-plugin-sdk contract test passed"
