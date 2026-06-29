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

}  // namespace satellite
