#include "satellite/manifest.hpp"

#include "satellite/json_io.hpp"

namespace satellite {

ManifestValidation validate_manifest_json(const nlohmann::json& manifest) {
    ManifestValidation             result;
    const std::vector<std::string> required = {
        "schema_version", "name", "executable", "version", "commands"};
    for (const auto& key : required) {
        if (!manifest.contains(key)) {
            result.message = "manifest missing required field: " + key;
            return result;
        }
    }
    result.ok      = true;
    result.message = "ok";
    return result;
}

nlohmann::json load_manifest_file(const std::filesystem::path& path) {
    return read_json_file(path);
}

bool manifests_equivalent(const nlohmann::json& runtime,
                          const nlohmann::json& on_disk) {
    const std::vector<std::string> keys = {"schema_version", "name",
                                           "executable", "version", "commands"};
    for (const auto& key : keys) {
        if (runtime.value(key, nlohmann::json()) !=
            on_disk.value(key, nlohmann::json())) {
            return false;
        }
    }
    return true;
}

}  // namespace satellite
