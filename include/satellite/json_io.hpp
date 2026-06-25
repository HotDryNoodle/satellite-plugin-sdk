#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace satellite {

using Json = nlohmann::json;

std::string read_text_file(const std::filesystem::path& path);
void write_text_file(const std::filesystem::path& path, const std::string& text);

Json read_json_file(const std::filesystem::path& path);
Json read_json_stdin();
Json read_json_input(const std::optional<std::filesystem::path>& input_path, bool use_stdin);

void write_json_stdout(const Json& value, bool pretty = false);
void write_json_file(const std::filesystem::path& path, const Json& value, bool pretty = false);

std::string utc_now_iso8601();

}  // namespace satellite
