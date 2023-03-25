#define FMT_HEADER_ONLY
#include <dtslib/filesystem.hpp>
#include <fmt/printf.h>

#include "Compiler.hpp"

void print_usage() { fmt::println("usage: rack <file.rack>"); }

int main(const int argc, const char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const std::string file = argv[1];

    auto compiler = Compiler::create(file);
    compiler.push_error(RackError{ .message = "test error",
                                   .span    = Span::create(file, 33, 36) });
    compiler.print_errors();

    return 0;
}
