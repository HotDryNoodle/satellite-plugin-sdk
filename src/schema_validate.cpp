#include "satellite/schema_validate.hpp"

#include <cstdlib>

#include "satellite/json_io.hpp"

namespace satellite {
namespace {

std::string json_type_name(const nlohmann::json& value) {
    if (value.is_null()) {
        return "null";
    }
    if (value.is_boolean()) {
        return "boolean";
    }
    if (value.is_number_integer() || value.is_number_unsigned()) {
        return "integer";
    }
    if (value.is_number_float()) {
        return "number";
    }
    if (value.is_string()) {
        return "string";
    }
    if (value.is_array()) {
        return "array";
    }
    if (value.is_object()) {
        return "object";
    }
    return "unknown";
}

bool type_matches(const nlohmann::json& value, const std::string& schema_type) {
    if (schema_type == "number") {
        return value.is_number();
    }
    if (schema_type == "integer") {
        return value.is_number_integer() || value.is_number_unsigned();
    }
    return json_type_name(value) == schema_type;
}

SchemaValidation fail(const std::string& message) {
    return SchemaValidation{false, message};
}

SchemaValidation validate_value(const nlohmann::json& instance,
                                const nlohmann::json& schema,
                                const std::string& path) {
    if (schema.contains("const")) {
        if (instance != schema.at("const")) {
            return fail(path + " must equal const value");
        }
    }

    if (schema.contains("enum")) {
        bool matched = false;
        for (const auto& item : schema.at("enum")) {
            if (instance == item) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            return fail(path + " is not in enum");
        }
    }

    if (schema.contains("type")) {
        const auto& type_node = schema.at("type");
        if (type_node.is_string()) {
            if (!type_matches(instance, type_node.get<std::string>())) {
                return fail(path + " expected type " + type_node.get<std::string>());
            }
        } else if (type_node.is_array()) {
            bool matched = false;
            for (const auto& t : type_node) {
                if (type_matches(instance, t.get<std::string>())) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return fail(path + " type mismatch");
            }
        }
    }

    if (schema.value("type", "") == "string" || instance.is_string()) {
        if (schema.contains("minLength")) {
            const auto min_len = schema.at("minLength").get<std::size_t>();
            if (instance.get<std::string>().size() < min_len) {
                return fail(path + " shorter than minLength");
            }
        }
    }

    if (instance.is_number()) {
        const double value = instance.get<double>();
        if (schema.contains("minimum") && value < schema.at("minimum").get<double>()) {
            return fail(path + " below minimum");
        }
        if (schema.contains("maximum") && value > schema.at("maximum").get<double>()) {
            return fail(path + " above maximum");
        }
    }

    if (schema.contains("required") && instance.is_object()) {
        for (const auto& key : schema.at("required")) {
            const auto name = key.get<std::string>();
            if (!instance.contains(name)) {
                return fail(path + " missing required field: " + name);
            }
        }
    }

    if (schema.contains("properties") && instance.is_object()) {
        const auto& properties = schema.at("properties");
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            const auto& key = it.key();
            if (!instance.contains(key)) {
                continue;
            }
            auto child = validate_value(instance.at(key), it.value(), path + "." + key);
            if (!child.ok) {
                return child;
            }
        }
    }

    if (schema.value("additionalProperties", true) == false && instance.is_object()) {
        for (auto it = instance.begin(); it != instance.end(); ++it) {
            if (!schema.contains("properties") || !schema.at("properties").contains(it.key())) {
                return fail(path + " has unexpected field: " + it.key());
            }
        }
    }

    return SchemaValidation{true, "ok"};
}

}  // namespace

std::filesystem::path default_schemas_dir() {
    if (const char* env = std::getenv("SATELLITE_SDK_SCHEMAS")) {
        return std::filesystem::path(env);
    }
    return std::filesystem::path(SATELLITE_SDK_SCHEMAS_DIR);
}

std::filesystem::path resolve_schema_path(const std::filesystem::path& name_or_path) {
    if (name_or_path.is_absolute() || name_or_path.has_parent_path()) {
        return name_or_path;
    }
    return default_schemas_dir() / name_or_path;
}

SchemaValidation validate_against_schema(const nlohmann::json& instance, const nlohmann::json& schema) {
    if (!schema.is_object()) {
        return fail("schema must be an object");
    }
    if (schema.value("type", "") == "object" && !instance.is_object()) {
        return fail("instance must be an object");
    }
    return validate_value(instance, schema, "$");
}

SchemaValidation validate_against_schema_file(const nlohmann::json& instance,
                                              const std::filesystem::path& schema_path) {
    if (!std::filesystem::exists(schema_path)) {
        return fail("schema file not found: " + schema_path.string());
    }
    const auto schema = read_json_file(schema_path);
    return validate_against_schema(instance, schema);
}

SchemaValidation resolve_schema_for_version(const std::string& schema_version,
                                            const std::string& kind,
                                            std::filesystem::path* out_schema_path) {
    const auto registry_path = default_schemas_dir() / "schema_registry.json";
    if (!std::filesystem::exists(registry_path)) {
        return fail("schema_registry.json not found: " + registry_path.string());
    }
    const auto registry = read_json_file(registry_path);
    if (!registry.contains("versions") || !registry.at("versions").is_object()) {
        return fail("schema_registry.json missing versions object");
    }
    const auto& versions = registry.at("versions");
    if (!versions.contains(schema_version) || !versions.at(schema_version).is_object()) {
        return fail("schema_registry has no entry for schema_version=" + schema_version);
    }
    const auto& entry = versions.at(schema_version);
    if (!entry.contains(kind) || !entry.at(kind).is_string()) {
        return fail("schema_registry version " + schema_version + " missing kind=" + kind);
    }
    const auto relative = entry.at(kind).get<std::string>();
    const auto resolved = default_schemas_dir() / relative;
    if (out_schema_path != nullptr) {
        *out_schema_path = resolved;
    }
    if (!std::filesystem::exists(resolved)) {
        return fail("registered schema file not found: " + resolved.string());
    }
    return SchemaValidation{true, resolved.string()};
}

SchemaValidation validate_by_schema_version(const nlohmann::json& instance,
                                            const std::string& kind) {
    if (!instance.is_object() || !instance.contains("schema_version") ||
        !instance.at("schema_version").is_string()) {
        return fail("instance missing schema_version string");
    }
    const auto version = instance.at("schema_version").get<std::string>();
    std::filesystem::path schema_path;
    auto resolved = resolve_schema_for_version(version, kind, &schema_path);
    if (!resolved.ok) {
        return resolved;
    }
    // Lightweight C++ validator: required/type/enum/const/additionalProperties only.
    // Full Draft 2020-12 ($ref/oneOf/if-then) coverage is in scripts/contract-test.sh.
    return validate_against_schema_file(instance, schema_path);
}

}  // namespace satellite
