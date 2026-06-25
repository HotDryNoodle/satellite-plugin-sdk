#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "PLUGIN_NAME.hpp"
#include "mp/common/exit_codes.hpp"
#include "mp/common/json_io.hpp"

namespace {

constexpr const char* kVersion = "0.1.0";
constexpr const char* kToolName = "TOOL_NAME";

struct CliOptions {
    std::string command;
    std::optional<std::filesystem::path> input_path;
    std::optional<std::filesystem::path> work_dir;
    bool use_stdin = false;
    bool dry_run = false;
    bool pretty = false;
    bool show_help = false;
};

void print_global_usage() {
    std::cerr << "PLUGIN_NAME " << kVersion << " - DESCRIPTION\n\n"
              << "Usage:\n"
              << "  PLUGIN_NAME manifest [--output json|json-pretty]\n"
              << "  PLUGIN_NAME validate (--input FILE | --stdin) [--output json|json-pretty]\n"
              << "  PLUGIN_NAME run (--input FILE | --stdin) --work-dir DIR [--dry-run] [--output json|json-pretty]\n\n"
              << "Global options:\n"
              << "  --help, -h     Show help\n"
              << "  --version      Print version\n\n"
              << "Examples:\n"
              << "  PLUGIN_NAME manifest --output json\n"
              << "  PLUGIN_NAME validate --input samples/PLUGIN_NAME_request.json\n"
              << "  PLUGIN_NAME run --input samples/PLUGIN_NAME_request.json --work-dir /tmp/PLUGIN_NAME_task --dry-run\n";
}

void print_manifest_usage() {
    std::cerr << "PLUGIN_NAME manifest\n\n"
              << "Examples:\n"
              << "  PLUGIN_NAME manifest --output json\n";
}

void print_validate_usage() {
    std::cerr << "PLUGIN_NAME validate\n\n"
              << "Examples:\n"
              << "  PLUGIN_NAME validate --input samples/PLUGIN_NAME_request.json\n"
              << "  cat samples/PLUGIN_NAME_request.json | PLUGIN_NAME validate --stdin\n";
}

void print_run_usage() {
    std::cerr << "PLUGIN_NAME run\n\n"
              << "Examples:\n"
              << "  PLUGIN_NAME run --input samples/PLUGIN_NAME_request.json --work-dir /tmp/PLUGIN_NAME_task\n"
              << "  PLUGIN_NAME run --stdin --work-dir /tmp/PLUGIN_NAME_task --dry-run\n";
}

void print_command_help(const std::string& command) {
    if (command == "manifest") {
        print_manifest_usage();
    } else if (command == "validate") {
        print_validate_usage();
    } else if (command == "run") {
        print_run_usage();
    } else {
        print_global_usage();
    }
}

bool parse_args(int argc, char** argv, CliOptions& opts) {
    if (argc < 2) {
        return false;
    }
    opts.command = argv[1];
    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) {
            opts.input_path = argv[++i];
        } else if (arg == "--work-dir" && i + 1 < argc) {
            opts.work_dir = argv[++i];
        } else if (arg == "--stdin") {
            opts.use_stdin = true;
        } else if (arg == "--dry-run") {
            opts.dry_run = true;
        } else if (arg == "--output" && i + 1 < argc) {
            opts.pretty = (argv[++i] == std::string("json-pretty"));
        } else if (arg == "--help" || arg == "-h") {
            opts.show_help = true;
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            return false;
        }
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc >= 2) {
        const std::string first = argv[1];
        if (first == "--version") {
            std::cout << "PLUGIN_NAME " << kVersion << '\n';
            return mp::EXIT_OK;
        }
        if (first == "--help" || first == "-h") {
            print_global_usage();
            return mp::EXIT_OK;
        }
    }

    CliOptions opts;
    if (!parse_args(argc, argv, opts)) {
        print_global_usage();
        return mp::EXIT_USAGE;
    }
    if (opts.show_help) {
        print_command_help(opts.command);
        return mp::EXIT_OK;
    }

    try {
        if (opts.command == "manifest") {
            mp::write_json_stdout(PLUGIN_CPP_NS::make_manifest(), opts.pretty);
            return mp::EXIT_OK;
        }

        if (opts.command == "validate") {
            if (!opts.input_path && !opts.use_stdin) {
                std::cerr << "Error: --input request.json or --stdin is required for validate\n";
                std::cerr << "  PLUGIN_NAME validate --input samples/PLUGIN_NAME_request.json\n";
                return mp::EXIT_VALIDATION;
            }
            const auto request = mp::read_json_input(opts.input_path, opts.use_stdin);
            const auto result = PLUGIN_CPP_NS::validate_request(request);
            mp::write_json_stdout({{"ok", result.ok}, {"message", result.message}, {"details", result.details}},
                                  opts.pretty);
            return result.ok ? mp::EXIT_OK : mp::EXIT_VALIDATION;
        }

        if (opts.command == "run") {
            if (!opts.work_dir) {
                std::cerr << "Error: --work-dir is required for run\n";
                std::cerr << "  PLUGIN_NAME run --input request.json --work-dir /tmp/task_xxx\n";
                return mp::EXIT_VALIDATION;
            }
            if (!opts.input_path && !opts.use_stdin) {
                std::cerr << "Error: --input request.json or --stdin is required for run\n";
                std::cerr << "  PLUGIN_NAME run --input request.json --work-dir /tmp/task_xxx\n";
                return mp::EXIT_VALIDATION;
            }
            const auto request = mp::read_json_input(opts.input_path, opts.use_stdin);
            PLUGIN_CPP_NS::RunContext ctx;
            ctx.work_dir = *opts.work_dir;
            ctx.dry_run = opts.dry_run;
            if (request.contains("request_id")) {
                ctx.task_id = request.at("request_id").get<std::string>();
            }
            if (request.contains("trace_id")) {
                ctx.trace_id = request.at("trace_id").get<std::string>();
            }
            const auto output = PLUGIN_CPP_NS::run_plugin(request, ctx);
            mp::write_json_stdout(output, opts.pretty);
            if (output.value("status", "") == "no_result") {
                return mp::EXIT_NO_RESULT;
            }
            return mp::EXIT_OK;
        }

        std::cerr << "Unknown command: " << opts.command << '\n';
        print_global_usage();
        return mp::EXIT_USAGE;
    } catch (const std::exception& ex) {
        mp::write_json_stdout({{"ok", false}, {"error", ex.what()}}, false);
        return mp::EXIT_RETRYABLE;
    }
}
