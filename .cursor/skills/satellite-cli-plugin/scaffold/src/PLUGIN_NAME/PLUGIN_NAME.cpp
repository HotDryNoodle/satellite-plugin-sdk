#include "PLUGIN_NAME.hpp"

#include <chrono>
#include <sstream>

#include "mp/common/json_io.hpp"

namespace PLUGIN_CPP_NS {

namespace {

std::string make_task_id() {
    const auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream ss;
    ss << "task_" << now;
    return ss.str();
}

}  // namespace

nlohmann::json make_manifest() {
    return mp::read_json_file("configs/plugins/PLUGIN_NAME.json");
}

ValidationResult validate_request(const nlohmann::json& request) {
    ValidationResult result;
    if (!request.contains("request_id")) {
        result.message = "request_id is required";
        return result;
    }
    result.ok = true;
    result.message = "ok";
    result.details = {{"tool_name", "TOOL_NAME"}};
    return result;
}

nlohmann::json run_plugin(const nlohmann::json& request, const RunContext& ctx) {
    const auto validation = validate_request(request);
    if (!validation.ok) {
        throw std::runtime_error(validation.message);
    }

    const auto task_id = ctx.task_id.empty() ? make_task_id() : ctx.task_id;
    const auto request_id = request.at("request_id").get<std::string>();

    std::filesystem::create_directories(ctx.work_dir);
    mp::write_json_file(ctx.work_dir / "request.json", request, true);

    nlohmann::json output = {
        {"task_id", task_id},
        {"request_id", request_id},
        {"tool_name", "TOOL_NAME"},
        {"status", ctx.dry_run ? "dry_run" : "succeeded"},
        {"summary", nlohmann::json::object()},
        {"artifacts", {{"work_dir", ctx.work_dir.string()}}},
        {"warnings", nlohmann::json::array({"PLUGIN_NAME is a skeleton stub; replace run_plugin() with real logic"})},
    };
    if (ctx.trace_id) {
        output["trace_id"] = *ctx.trace_id;
    }

    if (!ctx.dry_run) {
        const auto output_path = ctx.work_dir / "result.json";
        mp::write_json_file(output_path, output, true);
        output["artifacts"]["primary_output"] = output_path.string();
    }

    mp::write_json_file(ctx.work_dir / "result.json", output, true);
    return output;
}

}  // namespace PLUGIN_CPP_NS
