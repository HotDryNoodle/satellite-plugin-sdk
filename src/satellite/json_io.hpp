#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

/**
 * @file json_io.hpp
 * @brief JSON/文本 I/O 与 UTC 时间戳工具。
 *
 * 时间尺度：@c utc_now_iso8601() 返回 UTC，格式为 ISO-8601
 * @c %Y-%m-%dT%H:%M:%SZ（秒精度，无闰秒处理）。
 */

namespace satellite {

using Json = nlohmann::json;

/**
 * @brief 读取文本文件全文。
 * @param path 文件路径。
 * @return 文件内容。
 * @throws std::runtime_error 打开失败。
 */
std::string read_text_file(const std::filesystem::path& path);

/**
 * @brief 原子写入文本文件（临时文件 + rename + fsync）。
 * @param path 目标路径。
 * @param text 写入内容。
 * @throws std::runtime_error 写入或 fsync 失败。
 * @note 父目录 fsync 为 best-effort，失败不抛异常。
 */
void write_text_file(const std::filesystem::path& path,
                     const std::string&           text);

/** @brief 读取 JSON 文件。 */
Json read_json_file(const std::filesystem::path& path);

/**
 * @brief 从 stdin 读取 JSON。
 * @throws std::runtime_error stdin 为空。
 */
Json read_json_stdin();

/**
 * @brief 从文件或 stdin 读取 JSON 输入。
 * @param input_path 输入文件路径（@c use_stdin 为 false 时必填）。
 * @param use_stdin 为 true 时从 stdin 读取。
 */
Json read_json_input(const std::optional<std::filesystem::path>& input_path,
                     bool                                        use_stdin);

/** @brief 将 JSON 写入 stdout。 */
void write_json_stdout(const Json& value, bool pretty = false);

/** @brief 将 JSON 写入文件（经 @ref write_text_file 持久化）。 */
void write_json_file(const std::filesystem::path& path,
                     const Json&                  value,
                     bool                         pretty = false);

/**
 * @brief 当前 UTC 时刻的 ISO-8601 字符串。
 * @return 形如 @c 2026-07-11T05:30:00Z 的 UTC 时间戳。
 */
std::string utc_now_iso8601();

}  // namespace satellite
