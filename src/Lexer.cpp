#include "Lexer.hpp"

auto Token::create(
  const std::string& lexeme,
  const TokenType    type,
  const Span&        span
) -> Token {
    return { lexeme, type, span };
}

auto Token::lexeme() const -> std::string { return this->m_lexeme; }

auto Token::type() const -> TokenType { return this->m_type; }

auto Token::span() const -> Span { return this->m_span; }

auto Token::type_to_string() const -> std::string {
    static_assert(
      std::to_underlying(TokenType::Max) == 2,
      "[INTERNAL ERROR] Token::type_to_string(): Exhaustive handling of all "
      "enum "
      "variants is required"
    );

    switch (this->m_type) {
        case TokenType::Number:
            return "Number";
        case TokenType::Plus:
            return "Plus";
        default: {
            return "Unknown Token Type";
        }
    }
}

Token::Token(std::string lexeme, const TokenType type, const Span& span)
  : m_lexeme{ std::move(lexeme) },
    m_type{ type },
    m_span{ span } {}

auto Lexer::lex(const Compiler& compiler)
  -> std::expected<std::vector<Token>, LexError> {
    if (compiler.file_contents().empty()) {
        return std::unexpected(LexError::EmptySource);
    }

    std::vector<Token> tokens;
    Lexer              lexer(compiler);

    auto&& token = lexer.next();
    while (token.error() != LexError::Eof) {
        if (!token.has_value()) { return std::unexpected(LexError::Eof); }

        tokens.push_back(token.value());
        token = lexer.next();
    }

    return tokens;
}

Lexer::Lexer(const Compiler& compiler)
  : m_compiler{ compiler },
    m_source{ compiler.file_contents() },
    m_cursor{ 0 } {}

auto Lexer::span(const std::size_t start, const std::size_t end) const -> Span {
    return Span::create(this->m_compiler.target(), start, end);
}

auto Lexer::eof() const -> bool {
    return this->m_cursor >= this->m_source.size();
}

auto Lexer::peek() const -> std::expected<char, LexError> {
    if (this->eof()) { return std::unexpected(LexError::Eof); }
    return this->m_source[this->m_cursor];
}

auto Lexer::peek_ahead(const std::size_t offset) const
  -> std::expected<char, LexError> {
    if (this->m_cursor + offset >= this->m_source.size()) {
        return std::unexpected(LexError::Eof);
    }
    return this->m_source[m_cursor];
}

auto Lexer::next() -> std::expected<Token, LexError> {
    while (true) {
        const auto ch = this->peek();

        if (ch.has_value() && (bool)std::isspace(ch.value())) {
            ++this->m_cursor;
        } else if (!ch.has_value()) {
            return std::unexpected(ch.error());
        } else {
            break;
        }
    }

    const auto current_char = this->peek();
    if (!current_char.has_value()) { return std::unexpected(LexError::Eof); }

    static_assert(
      std::to_underlying(TokenType::Max) == 2,
      "[INTERNAL ERROR] Lexer::next(): Exhaustive handling of all enum "
      "variants is required"
    );

    switch (current_char.value()) {}

    return Token::create("2", TokenType::Number, this->span(0, 1));
}
