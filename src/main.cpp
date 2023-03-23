#include <argparse/argparse.hpp>


int main(const int argc, const char** argv) {
    argparse::ArgumentParser parser(argc, argv);
    parser.add_argument("file")
      .set_nargs(1)
      .set_type(argparse::ArgTypes::STRING)
      .set_flags(argparse::ArgFlags::REQUIRED)
      .set_help("path to file to compile");

    const auto args = parser.parse_args();
    const std::string file = args.at("file").as<std::string>();

    return 0;
}
