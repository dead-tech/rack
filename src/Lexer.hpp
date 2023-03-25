#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstdint>
#include <expected>
#include <string>
#include <utility>
#include <vector>

enum class LexError : std::uint64_t { Eof = 0, EmptySource, Max };

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

#endif // LEXER_HPP
