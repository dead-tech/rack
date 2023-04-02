#include "Assembler.hpp"

Assembler::Assembler(const std::string& output_filename)
  : m_output_file{ std::make_unique<std::ofstream>(
    std::ofstream(output_filename, std::ios::out | std::ios::trunc)
  ) } {}

void Assembler::writeln(const std::string_view str) {
    *this->m_output_file << str << '\n';
}

auto Assembler_x86_64::compile(
  const std::shared_ptr<Compiler>& compiler,
  const std::vector<Token>&        tokens
) -> std::expected<void, AssembleError> {
    const auto output_path     = std::filesystem::path(compiler->target());
    const auto parent_path     = output_path.parent_path();
    const auto output_filename = fmt::format(
      "{}/{}.asm", parent_path.string(), output_path.stem().string()
    );

    if (!std::filesystem::exists(parent_path)) {
        // TODO: Implement a way to push errors without span
        return std::unexpected(AssembleError::NoSuchFileOrDirectory);
    }

    Assembler_x86_64 assembler(compiler, tokens, output_filename);
    return assembler.compile_to_assembly();
}

Assembler_x86_64::Assembler_x86_64(
  const std::shared_ptr<Compiler>& compiler,
  const std::vector<Token>&        tokens,
  const std::string&               output_filename
)
  : Assembler(output_filename),
    m_compiler{ compiler },
    m_tokens{ tokens } {}

auto Assembler_x86_64::compile_to_assembly()
  -> std::expected<void, AssembleError> {
    // TODO: Make it an actual valid assembly with the boiler plate,
    //       initialization and stuff...

    // Things like: BITS64, section .text, global _start...
    this->generate_assembly_header();

    // Already defined functions like: print...
    this->generate_assembly_prelude();

    auto&& token = this->next();
    while (token.error() != AssembleError::Eof) {
        if (!token.has_value()) { return std::unexpected(token.error()); }
        token = this->next();
    }

    // Effective program entry point
    this->generate_assembly_start_label();

    // Data section
    this->generate_data_section();

    return {};
}

auto Assembler_x86_64::generate_assembly_prelude() -> void {
    // TODO: Create an assembly file containing the prelude and include that,
    //       instead of doings this the hard way

    // print
    this->writeln("print:");
    this->writeln("\tmov r9, -3689348814741910323");
    this->writeln("\tsub rsp, 40");
    this->writeln("\tmov BYTE [rsp+31], 10");
    this->writeln("\tlea rcx, [rsp+30]");
    this->writeln(".L2:");
    this->writeln("\tmov rax, rdi");
    this->writeln("\tlea r8, [rsp+32]");
    this->writeln("\tmul r9");
    this->writeln("\tmov rax, rdi");
    this->writeln("\tsub r8, rcx");
    this->writeln("\tshr rdx, 3");
    this->writeln("\tlea rsi, [rdx+rdx*4]");
    this->writeln("\tadd rsi, rsi");
    this->writeln("\tsub rax, rsi");
    this->writeln("\tadd eax, 48");
    this->writeln("\tmov BYTE [rcx], al");
    this->writeln("\tmov rax, rdi");
    this->writeln("\tmov rdi, rdx");
    this->writeln("\tmov rdx, rcx");
    this->writeln("\tsub rcx, 1");
    this->writeln("\tcmp rax, 9");
    this->writeln("\tja .L2");
    this->writeln("\tlea rax, [rsp+32]");
    this->writeln("\tmov edi, 1");
    this->writeln("\tsub rdx, rax");
    this->writeln("\txor eax, eax");
    this->writeln("\tlea rsi, [rsp+32+rdx]");
    this->writeln("\tmov rdx, r8");
    this->writeln("\tmov rax, 1");
    this->writeln("\tsyscall");
    this->writeln("\tadd rsp, 40");
    this->writeln("\tret\n");

    // puts
    this->writeln("puts:");
    this->writeln("\tmov rax, 1");
    this->writeln("\tmov rdi, 1");
    this->writeln("\tmov rsi, r9");
    this->writeln("\tmov rdx, r8");
    this->writeln("\tsyscall");
    this->writeln("\tret\n");
}

void Assembler_x86_64::generate_assembly_header() {
    this->writeln("BITS 64");
    this->writeln("section .text\n");
}

void Assembler_x86_64::generate_assembly_start_label() {
    this->writeln("global _start");
    this->writeln("_start:");

    this->writeln("\tcall func_main");
    this->writeln("\tmov rax, 60");
    this->writeln("\tmov rdi, 0");
    this->writeln("\tsyscall\n");
}

void Assembler_x86_64::generate_data_section() {
    this->writeln("section .data");

    // TODO: Change this to zip, iota when zip ships
    std::size_t idx = 0;
    for (const auto& str : this->m_strings) {
        this->writeln(fmt::format("\tstr_{}: db `{}`", std::to_string(idx), str)
        );
        ++idx;
    }
    this->writeln("\n");
}

auto Assembler_x86_64::span(const std::size_t start, const std::size_t end)
  const -> Span {
    return Span::create(this->m_compiler->target(), start, end);
}

void Assembler_x86_64::error(const std::string& message, const Span& span) {
    this->m_compiler->push_error(RackError{ message, span });
}

auto Assembler_x86_64::eof() const -> bool {
    return this->m_cursor >= this->m_tokens.size();
}

auto Assembler_x86_64::peek() const -> std::expected<Token, AssembleError> {
    if (this->eof()) { return std::unexpected(AssembleError::Eof); }
    return this->m_tokens[this->m_cursor];
}

auto Assembler_x86_64::peek_ahead(const std::size_t offset) const
  -> std::expected<Token, AssembleError> {
    if (this->m_cursor + offset >= this->m_tokens.size() - 1) {
        return std::unexpected(AssembleError::Eof);
    }
    return this->m_tokens[this->m_cursor + offset];
}

auto Assembler_x86_64::next() -> std::expected<void, AssembleError> {
    const auto& current_token = this->peek();
    if (!current_token.has_value()) {
        return std::unexpected(current_token.error());
    }

    const auto lexeme = current_token.value().lexeme();

    switch (current_token.value().type()) {
        case TokenType::KeywordOrIdentifier: {
            static_assert(
              std::to_underlying(TokenType::Max) == 10,
              "[INTERNAL ERROR] Assembler_x86_64::next() requires to handle "
              "all enum variants"
            );
            if (lexeme == "fn") {
                return this->compile_function();
            } else {
                // FIXME: Make this an unknown keyword/identifier error
                fmt::println("unimplemented {}, skipping compilation!", lexeme);
                ++this->m_cursor;
            }
            break;
        }
        default: {
            // FIXME: Make this an unknown token error
            fmt::println("unimplemented {}, skipping compilation!", lexeme);
            ++this->m_cursor;
        }
    }

    return {};
}

auto Assembler_x86_64::compile_function()
  -> std::expected<void, AssembleError> {
    // Skip "fn" token
    const auto fn_keyword = this->peek().value();
    ++this->m_cursor;

    // Get function identifier
    const auto function_name = this->peek();
    if (!function_name.has_value()) {
        this->error(
          "expected identifier after 'fn' keyword, function name is missing",
          fn_keyword.span()
        );
        return std::unexpected(AssembleError::MissingFunctionName);
    }
    ++this->m_cursor;

    // Write function label
    this->writeln(fmt::format("func_{}:", function_name->lexeme()));

    // Check if function has parameter list
    const auto has_params = [&]() -> bool {
        const auto next_token = this->peek();
        if (!next_token.has_value()) {
            this->error(
              "expected parameter list or return type after function name",
              function_name.value().span()
            );
            return false;
        }

        return next_token.value().type() == TokenType::MinusMinus;
    }();

    // Parse parameter list
    if (has_params) {
        FMT_ASSERT(false, "unimplemented parsing function parameter list\n");
        // const auto parameter_list = this->parse_function_parameter_list();
    }

    // Check return type
    const auto arrow = this->peek();
    if (!arrow.has_value() || arrow.value().type() != TokenType::Arrow) {
        // FIXME: This span is not always correct: it should be either the function name's
        //        span or the last function parameter's span
        this->error(
          "expected return type after function name or parameter list",
          function_name.value().span()
        );
        return std::unexpected(
          AssembleError::MissingFunctionParametersOrReturnType
        );
    }
    ++this->m_cursor;

    const auto return_type = this->peek();
    // FIXME: Check if return type is a valid return type
    if (!return_type.has_value()) {
        this->error(
          "expected return type after function name or parameter list",
          arrow.value().span()
        );
        return std::unexpected(
          AssembleError::MissingFunctionParametersOrReturnType
        );
    }
    ++this->m_cursor;

    const auto begin_token = this->peek();
    if (!begin_token.has_value()) {
        this->error(
          "expected begin after function return type",
          return_type.value().span()
        );
        return std::unexpected(AssembleError::NoBeginToken);
    }
    ++this->m_cursor;

    const auto function_body_result =
      this->compile_function_body(begin_token->span());

    const auto end_token = this->peek();
    if (!end_token.has_value()) {
        this->error(
          "expected end token after function body", this->peek()->span()
        );
        return std::unexpected(AssembleError::NoEndToken);
    }
    ++this->m_cursor;

    this->writeln("\tret\n");

    return {};
}

auto Assembler_x86_64::compile_function_body(const Span& begin_span)
  -> std::expected<void, AssembleError> {
    auto&& token = this->peek();
    if (!token.has_value()) {
        this->error("expected end token after function body", begin_span);
        return std::unexpected(AssembleError::NoEndToken);
    }

    while (token->lexeme() != "end") {
        switch (token->type()) {
            case TokenType::DoubleQuotedString: {
                this->compile_double_quoted_string(token.value());
                break;
            }
            case TokenType::KeywordOrIdentifier: {
                if (token->is_keyword()) {
                    const auto result = this->compile_keyword(token.value());
                    if (!result.has_value()) {
                        return std::unexpected(result.error());
                    }
                } else {
                    const auto result =
                      this->compile_function_call(token.value());
                    if (!result.has_value()) {
                        return std::unexpected(result.error());
                    }
                }
                break;
            }
            default: {
                ++this->m_cursor;
                this->error(
                  fmt::format(
                    "{} is not allowed in this context", token->type_to_string()
                  ),
                  token->span()
                );
            }
        }

        token = this->peek();
    }

    return {};
}

// FIXME: This currently assumes it cannot fail, but maybe it can (?)
void Assembler_x86_64::compile_double_quoted_string(const Token& token) {
    this->m_strings.push_back(token.lexeme());

    const auto string_size = [&]() -> std::size_t {
        std::size_t occurrences = 0;
        std::size_t start       = 0;
        while ((start = token.lexeme().find("\\n", start)) != std::string::npos
        ) {
            ++occurrences;
            start += 2;
        }
        return token.lexeme().size() - occurrences;
    }();

    this->writeln(fmt::format("\tmov rax, {}", std::to_string(string_size)));
    this->writeln("\tpush rax");
    this->writeln(
      fmt::format("\tpush str_{}", std::to_string(this->m_strings.size() - 1))
    );
    ++this->m_cursor;
}

auto Assembler_x86_64::compile_keyword([[maybe_unused]] const Token& token)
  -> std::expected<void, AssembleError> {
    fmt::print(
      stderr, "[INTERNAL ERROR] compile_keyword(): is not implemented yet\n"
    );
    ++this->m_cursor;
    return {};
}

auto Assembler_x86_64::compile_function_call(const Token& token)
  -> std::expected<void, AssembleError> {
    if (token.lexeme() == "print") {
        this->writeln("\tpop rdi");
        this->writeln("\tcall print");
    } else if (token.lexeme() == "puts") {
        this->writeln("\tpop r9");
        this->writeln("\tpop r8");
        this->writeln("\tcall puts");
    } else {
        this->error(
          fmt::format("undeclared function {}", token.lexeme()), token.span()
        );
        return std::unexpected(AssembleError::UndeclaredFunction);
    }

    ++this->m_cursor;
    return {};
}
