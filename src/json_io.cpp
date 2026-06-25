#include "satellite/json_io.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace satellite {

std::string read_text_file(const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void write_text_file(const std::filesystem::path& path, const std::string& text) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Failed to write file: " + path.string());
    }
    out << text;
}

Json read_json_file(const std::filesystem::path& path) {
    return Json::parse(read_text_file(path));
}

Json read_json_stdin() {
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    const auto text = ss.str();
    if (text.empty()) {
        throw std::runtime_error("Empty stdin; expected JSON payload");
    }
    return Json::parse(text);
}

Json read_json_input(const std::optional<std::filesystem::path>& input_path, bool use_stdin) {
    if (use_stdin) {
        return read_json_stdin();
    }
    if (!input_path) {
        throw std::runtime_error("Missing --input request.json or --stdin");
    }
    return read_json_file(*input_path);
}

void write_json_stdout(const Json& value, bool pretty) {
    if (pretty) {
        std::cout << value.dump(2) << '\n';
    } else {
        std::cout << value.dump() << '\n';
    }
}

void write_json_file(const std::filesystem::path& path, const Json& value, bool pretty) {
    write_text_file(path, pretty ? value.dump(2) : value.dump());
}

std::string utc_now_iso8601() {
    using clock = std::chrono::system_clock;
    const auto now = clock::now();
    const auto t = clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&t, &tm);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

}  // namespace satellite
