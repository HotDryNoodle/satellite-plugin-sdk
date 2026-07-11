#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

/**
 * @file cli_options.hpp
 * @brief 插件 CLI 通用参数解析。
 */

namespace satellite {

/** @brief stdout JSON 输出格式。 */
enum class OutputFormat { Json, JsonPretty };

/** @brief 插件 validate/run 子命令的通用 CLI 选项。 */
struct PluginCliOptions {
    std::optional<std::filesystem::path> input_path;
    bool                                 use_stdin = false;
    std::optional<std::filesystem::path> work_dir;
    bool                                 dry_run      = false;
    OutputFormat                         output       = OutputFormat::Json;
    bool                                 show_help    = false;
    bool                                 show_version = false;
};

/** @brief CLI 解析结果。 */
struct CliParseResult {
    bool             ok = false;
    std::string      error;
    PluginCliOptions options;
};

/**
 * @brief 解析插件通用 CLI 参数（--input/--stdin/--work-dir/--dry-run 等）。
 * @param argc argv 长度。
 * @param argv 命令行参数。
 * @param require_work_dir run 子命令时设为 true。
 */
CliParseResult parse_plugin_cli(int    argc,
                                char** argv,
                                bool   require_work_dir = false);

/** @brief 返回用法说明字符串。 */
std::string plugin_cli_usage(const char* executable);

}  // namespace satellite
