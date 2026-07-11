#pragma once

/**
 * @file exit_codes.hpp
 * @brief 插件 CLI 统一退出码（与 satellite-plugin-sdk 合同一致）。
 */

namespace satellite {

/** @brief 成功退出。 */
constexpr int EXIT_OK = 0;
/** @brief 输入/校验失败。 */
constexpr int EXIT_VALIDATION = 2;
/** @brief 外部依赖不可用。 */
constexpr int EXIT_DEPENDENCY = 3;
/** @brief 运行完成但无可用结果。 */
constexpr int EXIT_NO_RESULT = 4;
/** @brief 可重试的瞬时失败。 */
constexpr int EXIT_RETRYABLE = 5;
/** @brief 不可恢复的内部错误。 */
constexpr int EXIT_FATAL = 6;
/** @brief 命令行用法错误。 */
constexpr int EXIT_USAGE = 64;

}  // namespace satellite
