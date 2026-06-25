#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace PLUGIN_CPP_NS {

struct ValidationResult {
    bool ok = false;
    std::string message;
    nlohmann::json details = nlohmann::json::object();
};

struct RunContext {
    std::filesystem::path work_dir;
    bool dry_run = false;
    std::string task_id;
    std::optional<std::string> trace_id;
};

nlohmann::json make_manifest();
ValidationResult validate_request(const nlohmann::json& request);
nlohmann::json run_plugin(const nlohmann::json& request, const RunContext& ctx);

}  // namespace PLUGIN_CPP_NS
