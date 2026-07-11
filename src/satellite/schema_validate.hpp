#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

/**
 * @file schema_validate.hpp
 * @brief 轻量 JSON Schema 校验与版本路由。
 *
 * @note 完整 Draft 2020-12 覆盖由 contract-test 脚本负责；运行时仅校验
 *       required/type/enum/const/minimum/maximum/additionalProperties。
 */

namespace satellite {

/** @brief Schema 校验结果。 */
struct SchemaValidation {
    bool        ok = false;
    std::string message;
};

/** @brief 用内存中的 schema 对象校验 instance。 */
SchemaValidation validate_against_schema(const nlohmann::json& instance,
                                         const nlohmann::json& schema);

/** @brief 从 schema 文件路径校验 instance。 */
SchemaValidation validate_against_schema_file(
    const nlohmann::json& instance, const std::filesystem::path& schema_path);

/**
 * @brief 解析 schema 目录。
 * @return 优先 @c SATELLITE_SDK_SCHEMAS 环境变量，否则编译期
 *         @c SATELLITE_SDK_SCHEMAS_DIR 。
 */
std::filesystem::path default_schemas_dir();

/** @brief 将 schema 名或相对路径解析为绝对路径。 */
std::filesystem::path resolve_schema_path(
    const std::filesystem::path& name_or_path);

/**
 * @brief 通过 schema_registry.json 按版本解析 schema 文件路径。
 * @param schema_version 实例中的 schema_version 字段值。
 * @param kind 种类：@c "envelope" | @c "result" | @c "manifest" 。
 * @param out_schema_path 可选输出：解析到的 schema 文件路径。
 */
SchemaValidation resolve_schema_for_version(
    const std::string&     schema_version,
    const std::string&     kind,
    std::filesystem::path* out_schema_path);

/** @brief 读取 instance.schema_version 并校验对应 kind 的注册 schema。 */
SchemaValidation validate_by_schema_version(const nlohmann::json& instance,
                                            const std::string&    kind);

}  // namespace satellite
