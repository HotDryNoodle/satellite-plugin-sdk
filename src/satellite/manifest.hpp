#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

/**
 * @file manifest.hpp
 * @brief 插件 manifest 校验与加载。
 */

namespace satellite {

/** @brief manifest 校验结果。 */
struct ManifestValidation {
    bool        ok = false;
    std::string message;
};

/**
 * @brief 校验运行时 manifest JSON 是否包含合同必填字段。
 * @param manifest manifest 对象。
 */
ManifestValidation validate_manifest_json(const nlohmann::json& manifest);

/** @brief 从磁盘加载 manifest JSON。 */
nlohmann::json load_manifest_file(const std::filesystem::path& path);

/**
 * @brief 比较运行时与磁盘 manifest 关键字段是否一致。
 * @note 仅比较 schema_version、name、executable、version、commands。
 */
bool manifests_equivalent(const nlohmann::json& runtime,
                          const nlohmann::json& on_disk);

}  // namespace satellite
