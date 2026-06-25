#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace satellite {

nlohmann::json make_error_json(const std::string& error);
nlohmann::json make_run_envelope(const std::string& task_id,
                                 const std::string& request_id,
                                 const std::string& tool_name,
                                 const std::string& status,
                                 const nlohmann::json& summary = nlohmann::json::object(),
                                 const std::vector<std::string>& warnings = {});

}  // namespace satellite
