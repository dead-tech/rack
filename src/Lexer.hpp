#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstdint>
#include <expected>
#include <fmt/format.h>
#include <string>
#include <utility>
#include <vector>

enum class LexError : std::uint64_t { Eof = 0, EmptySource, Max };

// TODO: Maybe move this class into a utility file, as this could be useful to
//       have in other files, without having to include the whole Lexer
class Span {
  public:
    [[nodiscard]] static auto
      create(const std::size_t start, const std::size_t end) -> Span;
    [[nodiscard]] auto start() const -> std::size_t;
    [[nodiscard]] auto end() const -> std::size_t;

  private:
    Span(const std::size_t start, const std::size_t end);

    std::size_t m_start;
    std::size_t m_end;
};

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
    [[nodiscard]] static auto lex(const std::string& source)
      -> std::expected<std::vector<Token>, LexError>;

  private:
    explicit Lexer(std::string source);

    [[nodiscard]] auto eof() const -> bool;
    [[nodiscard]] auto peek() const -> std::expected<char, LexError>;
    [[nodiscard]] auto peek_ahead(const std::size_t offset) const
      -> std::expected<char, LexError>;
    [[nodiscard]] auto next() -> std::expected<Token, LexError>;

    std::string m_source;
    std::size_t m_cursor;
};

/// {fmt} Custom Formatters
template<>
struct fmt::formatter<Span> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Span& span, FormatContext& ctx) {
        return fmt::format_to(
          ctx.out(), "Span {{ start: {}, end: {} }}", span.start(), span.end()
        );
    }
};

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
