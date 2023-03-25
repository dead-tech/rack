#ifndef LEXER_HPP
#define LEXER_HPP

#define FMT_HEADER_ONLY

#include "Compiler.hpp"
#include "Utility.hpp"
#include <cstdint>
#include <expected>
#include <fmt/format.h>
#include <string>
#include <utility>
#include <vector>

enum class LexError : std::uint64_t { Eof = 0, EmptySource, Max };

enum class TokenType : std::uint64_t { Number = 0, Plus, Max };

class Token {
  public:
    [[nodiscard]] static auto
      create(const std::string& lexeme, const TokenType type, const Span& span)
        -> Token;

    [[nodiscard]] auto lexeme() const -> std::string;
    [[nodiscard]] auto type() const -> TokenType;
    [[nodiscard]] auto span() const -> Span;

    [[nodiscard]] auto type_to_string() const -> std::string;

  private:
    Token(std::string lexeme, const TokenType type, const Span& span);

    std::string m_lexeme;
    TokenType   m_type;
    Span        m_span;
};

class Lexer {
  public:
    [[nodiscard]] static auto lex(const Compiler& compiler)
      -> std::expected<std::vector<Token>, LexError>;

  private:
    explicit Lexer(const Compiler& compiler);

    [[nodiscard]] auto
      span(const std::size_t start, const std::size_t end) const -> Span;

    [[nodiscard]] auto eof() const -> bool;
    [[nodiscard]] auto peek() const -> std::expected<char, LexError>;
    [[nodiscard]] auto peek_ahead(const std::size_t offset) const
      -> std::expected<char, LexError>;
    [[nodiscard]] auto next() -> std::expected<Token, LexError>;

    Compiler    m_compiler;
    std::string m_source;
    std::size_t m_cursor;
};

/// {fmt} Custom Formatters
template<>
struct fmt::formatter<Token> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Token& token, FormatContext& ctx) {
        return fmt::format_to(
          ctx.out(),
          "Token {{ lexeme: {}, type: {}, span: {} }}",
          token.lexeme(),
          token.type_to_string(),
          token.span()
        );
    }
};

#endif // LEXER_HPP
