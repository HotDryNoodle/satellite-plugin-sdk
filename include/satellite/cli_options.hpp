#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace satellite {

enum class OutputFormat { Json, JsonPretty };

struct PluginCliOptions {
    std::optional<std::filesystem::path> input_path;
    bool use_stdin = false;
    std::optional<std::filesystem::path> work_dir;
    bool dry_run = false;
    OutputFormat output = OutputFormat::Json;
    bool show_help = false;
    bool show_version = false;
};

struct CliParseResult {
    bool ok = false;
    std::string error;
    PluginCliOptions options;
};

CliParseResult parse_plugin_cli(int argc, char** argv, bool require_work_dir = false);
std::string plugin_cli_usage(const char* executable);

}  // namespace satellite
