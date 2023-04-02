#include "Error.hpp"
#define FMT_HEADER_ONLY
#include <argparse/argparse.hpp>
#include <dtslib/filesystem.hpp>
#include <fmt/printf.h>

#include "Assembler.hpp"
#include "Compiler.hpp"
#include "Lexer.hpp"

// TODO: Create a flag to specify output file name
// TODO: Make it so nasm and the linker are invoked automatically
int main([[maybe_unused]] const int argc, const char** argv) {
    argparse::ArgumentParser parser("rack", "0.0.1");
    parser.add_argument("file").help("path to rack file to compile");
    parser.add_argument("-o", "--output").help("compiled binary output path");
    parser.add_argument("-s", "--generate-asm")
      .help("generate assembly intermediate file");
    parser.add_argument("-T", "--lexed-tokens")
      .help("prints lexed tokens to stdout");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        fmt::print(
          stderr, fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "
        );
        fmt::print(stderr, fmt::emphasis::bold, "{}\n", err.what());
        fmt::print(stderr, "{}\n", parser.help().str());
    }

    const std::shared_ptr<Compiler> compiler =
      Compiler::create(parser.get<std::string>("file"));
    const auto tokens = Lexer::lex(compiler);

    if (!tokens.has_value()) {
        fmt::print(stderr, "[INTERNAL ERROR] lex error: {}\n", tokens.error());
        compiler->print_errors();
        return 1;
    }

    // FIXME: Create a compiler flag to enable printing the lexed tokens
    // for (const auto& token : tokens.value()) { fmt::println("{}", token); }

    const auto compile_result =
      Assembler_x86_64::compile(compiler, tokens.value());

    if (!compile_result.has_value()) {
        fmt::print(
          stderr, "[INTERNAL ERROR] compile error: {}\n", compile_result.error()
        );
        compiler->print_errors();
        return 1;
    }

    // As a final stage, print compiler errors if present
    if (compiler->has_errors()) {
        compiler->print_errors();
        return 1;
    }

    return 0;
}
