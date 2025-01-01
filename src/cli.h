#ifndef CLI_H
#define CLI_H

#include <iostream>
#include <string>
#include <cstring>
#include <optional>

struct RenderOptions {
    std::optional<int> Scene {};
    bool adaptive_sampling {false};
    int image_width {400};
    double aspect_ratio {16.0 / 9.0};
    int samples_per_pixel {100};
    int burn_in {64};
    int check_every {64};
    double tolerance {0.01};
    std::optional<std::string> output_file {};
    bool output_density {false};
};

namespace CLI {
    static const RenderOptions DEFAULT_OPTIONS {};

    static void usage(const char* argv0) {
        std::cerr << "Usage: " << argv0 << " [options] <Scene>" << std::endl
            << "Options:" << std::endl
            << "  -h              Show this help message"
            << std::endl
            << "  -o <file>       Send output to <file> instead of stdout. "
            << "Don't provide an extension, it will be added automatically."
            << std::endl
            << "  -w <width>      Image width in pixels "
            << "(default: " << DEFAULT_OPTIONS.image_width << ")"
            << std::endl
            << "  -r <ratio>      Aspect ratio (width/height) "
            << "(default: " << DEFAULT_OPTIONS.aspect_ratio << ")"
            << std::endl
            << "  -s <samples>    Samples per pixel "
            << "(default: " << DEFAULT_OPTIONS.samples_per_pixel << ")"
            << std::endl
            << "  -a              Enable adaptive sampling"
            << std::endl
            << "    -b <samples>  Burn-in samples "
            << "(default: " << DEFAULT_OPTIONS.burn_in << ")"
            << std::endl
            << "    -c <samples>  Check every <samples> samples "
            << "(default: " << DEFAULT_OPTIONS.check_every << ")"
            << std::endl
            << "    -t <tol>      Tolerance for adaptive sampling "
            << "(default: " << DEFAULT_OPTIONS.tolerance << ")"
            << std::endl
            << "    -d      Output sampling density image. "
            << "Only works if -a and -o are also specified."
            << std::endl;
    }

    static bool isdigit(char c) {
        return c >= '0' && c <= '9';
    }

    static bool isint(const char* str) {
        for (const char* c = str; *c; c++) {
            if (!isdigit(*c)) {
                return false;
            }
        }
        return true;
    }

    static bool parse_int(const char* str, int& value) {
        try {
            value = std::stoi(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    static bool parse_double(const char* str, double& value) {
        try {
            value = std::stod(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    void check_next_arg(int i, int argc, char* argv[]) {
        if (i + 1 >= argc) {
            std::cerr << "Error: -" << argv[i] << " requires a value" << std::endl;
            usage(argv[0]);
            exit(1);
        }
    }

    int parse_int_field(int& i, int argc, char* argv[]) {
        check_next_arg(i, argc, argv);
        int value {};
        // Consume the next argument, exiting if it's not a valid integer
        if (!parse_int(argv[++i], value)) {
            std::cerr << "Error: Invalid value for -" << argv[i] << std::endl;
            usage(argv[0]);
            exit(1);
        }
        return value;
    }

    double parse_double_field(int& i, int argc, char* argv[]) {
        check_next_arg(i, argc, argv);
        double value {};
        // Consume the next argument, exiting if it's not a valid double
        if (!parse_double(argv[++i], value)) {
            std::cerr << "Error: Invalid value for -" << argv[i] << std::endl;
            usage(argv[0]);
            exit(1);
        }
        return value;
    }

    static std::string parse_string_field(int& i, int argc, char* argv[]) {
        check_next_arg(i, argc, argv);
        const auto arg { argv[++i] };

        if (!arg) {
            std::cerr
                << "Error: -" << argv[i - 1] << " requires a value"
                << std::endl;
            usage(argv[0]);
            exit(1);
        }

        const auto len { std::strlen(arg) };
        if (len == 0) {
            std::cerr
                << "Error: -" << argv[i - 1] << " requires a value"
                << std::endl;
            usage(argv[0]);
            exit(1);
        }
        if (len > 4096) {
            std::cerr
                << "Error: -" << argv[i - 1] << " value too long"
                << std::endl;
            usage(argv[0]);
            exit(1);
        }

        return std::string(arg);
    }

    RenderOptions parse_args(int argc, char* argv[]) {
        RenderOptions options {};

        for (int i = 1; i < argc; i++) {
            if (isint(argv[i])) {
                options.Scene = std::stoi(argv[i]);
            } else if (strcmp(argv[i], "-a") == 0) {
                options.adaptive_sampling = true;
            } else if (strcmp(argv[i], "-w") == 0) {
                options.image_width = parse_int_field(i, argc, argv);
            } else if (strcmp(argv[i], "-r") == 0) {
                options.aspect_ratio = parse_double_field(i, argc, argv);
            } else if (strcmp(argv[i], "-s") == 0) {
                options.samples_per_pixel = parse_int_field(i, argc, argv);
            } else if (strcmp(argv[i], "-b") == 0) {
                options.burn_in = parse_int_field(i, argc, argv);
            } else if (strcmp(argv[i], "-c") == 0) {
                options.check_every = parse_int_field(i, argc, argv);
            } else if (strcmp(argv[i], "-t") == 0) {
                options.tolerance = parse_double_field(i, argc, argv);
            } else if (strcmp(argv[i], "-o") == 0) {
                options.output_file = parse_string_field(i, argc, argv);
            } else if (strcmp(argv[i], "-h") == 0) {
                usage(argv[0]);
                exit(0);
            } else if (strcmp(argv[i], "-d") == 0) {
                options.output_density = true;
            } else {
                std::cerr << "Error: Unknown option: " << argv[i] << std::endl;
                usage(argv[0]);
                exit(1);
            }
        }

        return options;
    }
}

#endif // CLI_H
