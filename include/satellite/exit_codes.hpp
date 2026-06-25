#pragma once

namespace satellite {

constexpr int EXIT_OK = 0;
constexpr int EXIT_VALIDATION = 2;
constexpr int EXIT_DEPENDENCY = 3;
constexpr int EXIT_NO_RESULT = 4;
constexpr int EXIT_RETRYABLE = 5;
constexpr int EXIT_FATAL = 6;
constexpr int EXIT_USAGE = 64;

}  // namespace satellite
