#define FMT_HEADER_ONLY
#include <argparse/argparse.hpp>
#include <dtslib/filesystem.hpp>
#include <fmt/printf.h>

#include "Compiler.hpp"

int main(const int argc, const char** argv) {
    argparse::ArgumentParser parser(argc, argv);
    parser.add_argument("file")
      .set_nargs(1)
      .set_type(argparse::ArgTypes::STRING)
      .set_flags(argparse::ArgFlags::REQUIRED)
      .set_help("path to file to compile");

    const auto        args = parser.parse_args();
    const std::string file = args.at("file").as<std::string>();

    // FIXME: Find out why removing this print statement is making the compiler
    //        crash
    fmt::print("");
    auto compiler = Compiler::create(file);
    compiler.push_error(RackError{ .message = "test error",
                                   .span    = Span::create(file, 33, 36) });
    compiler.print_errors();

    return 0;
}
