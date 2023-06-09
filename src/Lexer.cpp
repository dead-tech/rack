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
      std::to_underlying(TokenType::Max) == 10,
      "[INTERNAL ERROR] Token::type_to_string(): Exhaustive handling of all "
      "enum "
      "variants is required"
    );

    switch (this->m_type) {
        case TokenType::Number:
            return "Number";
        case TokenType::Plus:
            return "Plus";
        case TokenType::KeywordOrIdentifier: {
            return "KeywordOrIdentifier";
        }
        case TokenType::Minus: {
            return "Minus";
        }
        case TokenType::MinusMinus: {
            return "MinusMinus";
        }
        case TokenType::Colon: {
            return "Colon";
        }
        case TokenType::Comma: {
            return "Comma";
        }
        case TokenType::Asterisk: {
            return "Asterisk";
        }
        case TokenType::Arrow: {
            return "Arrow";
        }
        case TokenType::DoubleQuotedString: {
            return "DoubleQuotedString";
        }
        default: {
            return "Unknown Token Type";
        }
    }
}

auto Token::is_keyword() const -> bool {
    constexpr static std::array<std::string_view, 3> keywords = { "fn",
                                                                  "begin",
                                                                  "end" };
    return std::ranges::find(keywords, this->m_lexeme) != keywords.end();
}

Token::Token(std::string lexeme, const TokenType type, const Span& span)
  : m_lexeme{ std::move(lexeme) },
    m_type{ type },
    m_span{ span } {}

auto Lexer::lex(const std::shared_ptr<Compiler>& compiler)
  -> std::expected<std::vector<Token>, LexError> {
    if (compiler->file_contents().empty()) {
        return std::unexpected(LexError::EmptySource);
    }

    std::vector<Token> tokens;
    Lexer              lexer(compiler);

    auto&& token = lexer.next();
    while (token.error() != LexError::Eof) {
        if (!token.has_value()) { return std::unexpected(token.error()); }

        tokens.push_back(token.value());
        token = lexer.next();
    }

    return tokens;
}

Lexer::Lexer(const std::shared_ptr<Compiler>& compiler)
  : m_compiler{ compiler },
    m_source{ compiler->file_contents() },
    m_cursor{ 0 } {}

auto Lexer::span(const std::size_t start, const std::size_t end) const -> Span {
    return Span::create(this->m_compiler->target(), start, end);
}

void Lexer::error(const std::string& message, const Span& span) {
    this->m_compiler->push_error(RackError{ message, span });
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
    if (!current_char.has_value()) {
        return std::unexpected(current_char.error());
    }

    const auto start = this->m_cursor;
    switch (current_char.value()) {
        case ':': {
            return Token::create(
              ":", TokenType::Colon, this->span(start, this->m_cursor++)
            );
        }
        case ',': {
            return Token::create(
              ",", TokenType::Comma, this->span(start, this->m_cursor++)
            );
        }
        case '*': {
            return Token::create(
              "*", TokenType::Asterisk, this->span(start, this->m_cursor++)
            );
        }
        case '-': {
            return this->lex_minus();
        }
        case '"': {
            return this->lex_quoted_string();
        }
        default: {
            return this->lex_keyword_identifier_or_number();
        }
    }
}

auto Lexer::lex_keyword_identifier_or_number()
  -> std::expected<Token, LexError> {
    const auto start = this->m_cursor;
    if (this->eof()) {
        this->error("unexpected eof", this->span(start, start));
        return std::unexpected(LexError::Eof);
    }

    // This should be guaranteed as we have just checked for eof
    auto       current_char = this->peek().value();
    const auto is_valid_char_for_identifier_or_keyword =
      [](const auto ch) -> bool { return std::isalnum(ch) != 0 || ch == '_'; };

    if (std::isdigit(current_char) != 0) {
        return this->lex_number();
    } else if (std::isalpha(current_char) != 0 || current_char == '_') {
        std::stringstream ss;
        while (!this->eof()
               && is_valid_char_for_identifier_or_keyword(current_char)) {
            ss << current_char;
            ++this->m_cursor;
            current_char = this->peek().value();
        }

        return Token::create(
          ss.str(),
          TokenType::KeywordOrIdentifier,
          this->span(start, this->m_cursor - 1)
        );
    }

    this->error(
      fmt::format("unexpected character {}", current_char),
      this->span(start, ++this->m_cursor)
    );
    return std::unexpected(LexError::UnexpectedCharacter);
}

auto Lexer::lex_number() -> std::expected<Token, LexError> {
    const auto start = this->m_cursor;
    if (this->eof()) {
        this->error("unexpected eof", this->span(start, start));
        return std::unexpected(LexError::Eof);
    }

    // TODO: Add support for hex, octal and binary
    constexpr std::string_view valid_digits   = "0123456789";
    const auto                 is_valid_digit = [&](const auto ch) -> bool {
        return valid_digits.find(ch) != std::string::npos;
    };

    std::stringstream number;

    // TODO: Handle floating point numbers, digit separators, prefix literals,
    //       suffix literals
    auto current_char = this->peek().value();
    while (!this->eof() && is_valid_digit(current_char)) {
        current_char = this->peek().value();
        number << current_char;
        ++this->m_cursor;
    }

    return Token::create(
      number.str(), TokenType::Number, this->span(start, this->m_cursor - 1)
    );
}

auto Lexer::lex_minus() -> std::expected<Token, LexError> {
    const auto start = this->m_cursor;
    ++this->m_cursor;
    const auto next_char = this->peek();

    if (next_char.has_value()) {
        switch (next_char.value()) {
            case '-': {
                return Token::create(
                  "--",
                  TokenType::MinusMinus,
                  this->span(start, this->m_cursor++)
                );
            }
            case '>': {
                return Token::create(
                  "->", TokenType::Arrow, this->span(start, this->m_cursor++)
                );
            }
            default: {
                this->error(
                  fmt::format(
                    "unexpected char: {} after '-' ", next_char.value()
                  ),
                  this->span(start, this->m_cursor++)
                );
                return std::unexpected(LexError::UnexpectedCharacter);
            }
        }
    } else {
        return Token::create(
          "-", TokenType::Minus, this->span(start, this->m_cursor++)
        );
    }
}

auto Lexer::lex_quoted_string() -> std::expected<Token, LexError> {
    // Skip first double quote
    const auto start = this->m_cursor;
    ++this->m_cursor;

    // This means the string literal is not terminated
    if (this->eof()) {
        this->error(
          "unexpected eof: unterminated string literal",
          this->span(start, this->m_cursor)
        );
        return std::unexpected(LexError::Eof);
    }

    // This should be guaranteed as we have checked just above for eof
    auto current_char = this->peek().value();
    bool is_escaped   = false;

    // TODO: Check against a proper delimiter, if we want to support for single
    //       quoted strings
    while (!this->eof() && (is_escaped || current_char != '"')) {
        current_char = this->peek().value();

        if (current_char == '\r' || current_char == '\n') {
            ++this->m_cursor;
            continue;
        }

        is_escaped = !is_escaped && current_char == '\\';
        ++this->m_cursor;
    }

    const auto quoted =
      this->m_source.substr(start + 1, this->m_cursor - start - 2);

    return Token::create(
      quoted, TokenType::DoubleQuotedString, this->span(start, this->m_cursor++)
    );
}
