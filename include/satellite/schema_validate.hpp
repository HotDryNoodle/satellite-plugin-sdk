#pragma once

#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

namespace satellite {

struct SchemaValidation {
    bool ok = false;
    std::string message;
};

SchemaValidation validate_against_schema(const nlohmann::json& instance, const nlohmann::json& schema);
SchemaValidation validate_against_schema_file(const nlohmann::json& instance,
                                              const std::filesystem::path& schema_path);

std::filesystem::path default_schemas_dir();
std::filesystem::path resolve_schema_path(const std::filesystem::path& name_or_path);

// Version routing via schemas/schema_registry.json (inventory only; no runtime policy).
// kind: "envelope" | "result" | "manifest"
SchemaValidation resolve_schema_for_version(const std::string& schema_version,
                                            const std::string& kind,
                                            std::filesystem::path* out_schema_path);

SchemaValidation validate_by_schema_version(const nlohmann::json& instance,
                                            const std::string& kind);

}  // namespace satellite
