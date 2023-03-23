#define FMT_HEADER_ONLY
#include <argparse/argparse.hpp>
#include <dtslib/filesystem.hpp>
#include <fmt/printf.h>


int main(const int argc, const char** argv) {
    argparse::ArgumentParser parser(argc, argv);
    parser.add_argument("file")
      .set_nargs(1)
      .set_type(argparse::ArgTypes::STRING)
      .set_flags(argparse::ArgFlags::REQUIRED)
      .set_help("path to file to compile");

    const auto        args = parser.parse_args();
    const std::string file = args.at("file").as<std::string>();

    const auto file_contents = dts::read_file<std::string>(file);

    if (!file_contents.has_value()) {
        fmt::print(
          "[ERROR]: error occurred while reading file {}: {}\n",
          file,
          dts::filesystem_error(file_contents.error())
        );
        return 1;
    }

    return 0;
}
