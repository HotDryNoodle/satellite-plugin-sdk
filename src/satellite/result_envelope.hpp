#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

/**
 * @file result_envelope.hpp
 * @brief TaskManager 结果信封构造辅助。
 */

namespace satellite {

/** @brief 构造标准错误 JSON：@c {"ok":false,"error":...} 。 */
nlohmann::json make_error_json(const std::string& error);

/**
 * @brief 构造 run 结果信封骨架。
 * @param task_id 任务 ID。
 * @param request_id 请求 ID。
 * @param tool_name 工具名。
 * @param status 状态字符串（合同定义）。
 * @param summary 摘要对象。
 * @param warnings 警告列表。
 */
nlohmann::json make_run_envelope(
    const std::string&              task_id,
    const std::string&              request_id,
    const std::string&              tool_name,
    const std::string&              status,
    const nlohmann::json&           summary  = nlohmann::json::object(),
    const std::vector<std::string>& warnings = {});

}  // namespace satellite
