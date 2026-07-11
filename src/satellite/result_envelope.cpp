#include "satellite/result_envelope.hpp"

namespace satellite {

nlohmann::json make_error_json(const std::string& error) {
    return {{"ok", false}, {"error", error}};
}

nlohmann::json make_run_envelope(const std::string&              task_id,
                                 const std::string&              request_id,
                                 const std::string&              tool_name,
                                 const std::string&              status,
                                 const nlohmann::json&           summary,
                                 const std::vector<std::string>& warnings) {
    nlohmann::json out = {
        {"task_id", task_id},     {"request_id", request_id},
        {"tool_name", tool_name}, {"status", status},
        {"summary", summary},     {"artifacts", nlohmann::json::object()},
        {"warnings", warnings},
    };
    return out;
}

}  // namespace satellite
