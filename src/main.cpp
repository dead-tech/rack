#include "Error.hpp"
#define FMT_HEADER_ONLY
#include <argparse/argparse.hpp>
#include <dtslib/filesystem.hpp>
#include <fmt/printf.h>

#include "Assembler.hpp"
#include "Compiler.hpp"
#include "Lexer.hpp"

static bool
  invoke_external_command(const std::string& command, const bool verbose) {
    const auto start = std::chrono::steady_clock::now();

    const auto executable_name = [&]() {
        if (const auto space_index = command.find(' ');
            space_index != std::string::npos) {
            return command.substr(0, space_index);
        }
        return command;
    }();

    // FIXME: There has to be a better way to do this
    FILE* executable_process = popen(command.c_str(), "r");
    if (executable_process == nullptr) {
        fmt::print(
          stderr, fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "
        );
        fmt::print(
          stderr, fmt::emphasis::bold, "{} not found\n", executable_name
        );
        return false;
    }

    pclose(executable_process);

    const auto end = std::chrono::steady_clock::now();
    if (verbose) {
        fmt::print(
          stdout,
          "[INFO] {}........{:.2f}s\n",
          command,
          static_cast<std::chrono::duration<double>>(end - start).count()
        );
    }

    return true;
}

int main(const int argc, const char** argv) {
    argparse::ArgumentParser parser("rack", "0.0.1");
    parser.add_argument("file").help("path to rack file to compile");
    parser.add_argument("-o", "--output").help("compiled binary output path");
    parser.add_argument("-s", "--generate-asm")
      .help("generate assembly intermediate file")
      .default_value(false)
      .implicit_value(true);
    parser.add_argument("-T", "--lexed-tokens")
      .help("prints lexed tokens to stdout")
      .default_value(false)
      .implicit_value(true)
      .help("prints lexed tokens to stdout");
    parser.add_argument("-V", "--verbose")
      .default_value(false)
      .implicit_value(true)
      .help("print compilation phases, and executed commands to stdout");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        fmt::print(
          stderr, fmt::fg(fmt::color::red) | fmt::emphasis::bold, "error: "
        );
        fmt::print(stderr, fmt::emphasis::bold, "{}\n", err.what());
        fmt::print(stderr, "{}\n", parser.help().str());
    }

    const auto verbose = parser.get<bool>("--verbose");

    auto              input_file      = parser.get<std::string>("file");
    const auto        input_file_path = std::filesystem::path(input_file);
    const std::string input_file_path_without_extension =
      input_file_path.parent_path() / input_file_path.stem();

    const auto output_file = [&]() {
        if (const auto output = parser.present("--output")) {
            return parser.get<std::string>("--output");
        } else {
            return input_file;
        }
    }();

    const auto        output_file_path = std::filesystem::path(output_file);
    const std::string output_file_path_without_extension =
      output_file_path.parent_path() / output_file_path.stem();

    const auto compilation_start = std::chrono::steady_clock::now();

    const std::shared_ptr<Compiler> compiler =
      Compiler::create(input_file, output_file);
    const auto tokens = Lexer::lex(compiler);

    if (!tokens.has_value()) {
        fmt::print(stderr, "[INTERNAL ERROR] lex error: {}\n", tokens.error());
        compiler->print_errors();
        return 1;
    }

    if (parser["--lexed-tokens"] == true) {
        for (const auto& token : tokens.value()) { fmt::println("{}", token); }
    }

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

    const auto compilation_end = std::chrono::steady_clock::now();

    if (verbose) {
        fmt::print(
          stdout,
          "[INFO] Compiled in........{:.2f}s\n",
          static_cast<std::chrono::duration<double>>(
            compilation_end - compilation_start
          )
            .count()
        );
    }

    const std::string output_assembly_file =
      fmt::format("{}.asm", output_file_path_without_extension);
    const std::string output_object_file =
      fmt::format("{}.o", output_file_path_without_extension);

    // Now we can invoke nasm and then link
    const std::string nasm_command = fmt::format(
      "nasm -f elf64 {} -o {}", output_assembly_file, output_object_file
    );

    if (!invoke_external_command(nasm_command, verbose)) { return 1; }

    const std::string ld_command =
      fmt::format("ld {} -o {}", output_object_file, output_file_path.string());

    if (!invoke_external_command(ld_command, verbose)) { return 1; }

    // Cleanup (delete intermediate files)

    const std::string cleanup_command = [&]() {
        if (parser["--generate-asm"] == true) {
            return fmt::format("rm {}", output_object_file);
        } else {
            return fmt::format(
              "rm {} {}", output_object_file, output_assembly_file
            );
        }
    }();

    if (!invoke_external_command(cleanup_command, verbose)) { return 1; }

    return 0;
}
