#include "satellite/cli_options.hpp"

#include <iostream>
#include <string>

namespace satellite {

std::string plugin_cli_usage(const char* executable) {
    return std::string(executable) +
           " validate (--input FILE | --stdin) [--output json|json-pretty]\n"
           "  " +
           executable +
           " run (--input FILE | --stdin) --work-dir DIR [--dry-run] [--output json|json-pretty]\n";
}

CliParseResult parse_plugin_cli(int argc, char** argv, bool require_work_dir) {
    CliParseResult result;
    result.ok = true;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            result.options.show_help = true;
            return result;
        }
        if (arg == "--version") {
            result.options.show_version = true;
            return result;
        }
        if (arg == "--input" && i + 1 < argc) {
            result.options.input_path = argv[++i];
            continue;
        }
        if (arg == "--stdin") {
            result.options.use_stdin = true;
            continue;
        }
        if (arg == "--work-dir" && i + 1 < argc) {
            result.options.work_dir = argv[++i];
            continue;
        }
        if (arg == "--dry-run") {
            result.options.dry_run = true;
            continue;
        }
        if (arg == "--output" && i + 1 < argc) {
            const std::string fmt = argv[++i];
            if (fmt == "json-pretty") {
                result.options.output = OutputFormat::JsonPretty;
            } else if (fmt == "json") {
                result.options.output = OutputFormat::Json;
            } else {
                result.ok = false;
                result.error = "unsupported --output format: " + fmt;
                return result;
            }
            continue;
        }
        result.ok = false;
        result.error = "unknown argument: " + arg;
        return result;
    }

    if (require_work_dir && !result.options.work_dir.has_value()) {
        result.ok = false;
        result.error = "--work-dir is required";
        return result;
    }

    if (!result.options.use_stdin && !result.options.input_path.has_value()) {
        result.ok = false;
        result.error = "--input FILE or --stdin is required";
        return result;
    }

    return result;
}

}  // namespace satellite
