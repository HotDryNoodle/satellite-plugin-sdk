#pragma once

#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

namespace satellite {

struct ManifestValidation {
    bool ok = false;
    std::string message;
};

ManifestValidation validate_manifest_json(const nlohmann::json& manifest);
nlohmann::json load_manifest_file(const std::filesystem::path& path);
bool manifests_equivalent(const nlohmann::json& runtime, const nlohmann::json& on_disk);

}  // namespace satellite
