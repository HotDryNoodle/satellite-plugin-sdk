#include "satellite/json_io.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

namespace satellite {
namespace {

std::string make_tmp_suffix() {
    static thread_local std::mt19937_64     rng{std::random_device{}()};
    std::uniform_int_distribution<uint64_t> dist;
    std::ostringstream                      oss;
    oss << ".tmp." << getpid() << '.' << std::hex << dist(rng);
    return oss.str();
}

void fsync_path(const std::filesystem::path& path) {
    // 父目录 fsync 为 best-effort：open 失败时静默返回。
    const int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) { return; }
    ::fsync(fd);
    ::close(fd);
}

}  // namespace

std::string read_text_file(const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void write_text_file(const std::filesystem::path& path,
                     const std::string&           text) {
    std::filesystem::create_directories(path.parent_path());
    const auto tmp = path.string() + make_tmp_suffix();
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("Failed to write temp file: " + tmp);
        }
        out << text;
        out.flush();
        if (!out) {
            std::filesystem::remove(tmp);
            throw std::runtime_error("Failed to flush temp file: " + tmp);
        }
    }
    // fsync file contents before rename so crash cannot leave truncated final
    // path.
    {
        const int fd = ::open(tmp.c_str(), O_RDWR);
        if (fd >= 0) {
            if (::fsync(fd) != 0) {
                ::close(fd);
                std::filesystem::remove(tmp);
                throw std::runtime_error("Failed to fsync temp file: " + tmp);
            }
            ::close(fd);
        }
    }
    std::error_code ec;
    std::filesystem::rename(tmp, path, ec);
    if (ec) {
        std::filesystem::remove(tmp);
        throw std::runtime_error("Failed to rename temp file onto: " +
                                 path.string() + " (" + ec.message() + ")");
    }
    // Best-effort 目录 fsync，保证 rename 持久化。
    fsync_path(path.parent_path());
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

Json read_json_input(const std::optional<std::filesystem::path>& input_path,
                     bool                                        use_stdin) {
    if (use_stdin) { return read_json_stdin(); }
    if (!input_path) {
        throw std::runtime_error("Missing --input request.json or --stdin");
    }
    return read_json_file(*input_path);
}

void write_json_stdout(const Json& value, bool pretty) {
    if (pretty) { std::cout << value.dump(2) << '\n'; }
    else { std::cout << value.dump() << '\n'; }
}

void write_json_file(const std::filesystem::path& path,
                     const Json&                  value,
                     bool                         pretty) {
    write_text_file(path, pretty ? value.dump(2) : value.dump());
}

std::string utc_now_iso8601() {
    using clock    = std::chrono::system_clock;
    const auto now = clock::now();
    const auto t   = clock::to_time_t(now);
    std::tm    tm{};
    gmtime_r(&t, &tm);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

}  // namespace satellite
