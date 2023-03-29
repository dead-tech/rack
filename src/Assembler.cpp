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
    auto&& token = this->next();
    while (token.error() != AssembleError::Eof) {
        if (!token.has_value()) { return std::unexpected(token.error()); }
        token = this->next();
    }

    return {};
}

auto Assembler_x86_64::span(const std::size_t start, const std::size_t end)
  const -> Span {
    return Span::create(this->m_compiler->target(), start, end);
}

void Assembler_x86_64::error(const std::string& message, const Span& span) {
    this->m_compiler->push_error(RackError{ message, span });
}

auto Assembler_x86_64::eof() const -> bool {
    return this->m_cursor >= this->m_tokens.size() - 1;
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
    this->writeln("compile_function!");
    ++this->m_cursor;

    return {};
}
