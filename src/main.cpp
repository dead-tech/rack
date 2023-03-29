#define FMT_HEADER_ONLY
#include <dtslib/filesystem.hpp>
#include <fmt/printf.h>

#include "Assembler.hpp"
#include "Compiler.hpp"
#include "Lexer.hpp"

void print_usage() { fmt::println("usage: rack <file.rack>"); }

int main(const int argc, const char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const std::string file = argv[1];

    const std::shared_ptr<Compiler> compiler = Compiler::create(file);
    const auto                      tokens   = Lexer::lex(compiler);

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

    // TODO: Add a shortcut such as compiler.has_errors()
    // As a final stage, print compiler errors if present
    if (!compiler->errors().empty()) {
        compiler->print_errors();
        return 1;
    }

    return 0;
}
