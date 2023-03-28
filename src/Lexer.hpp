#ifndef LEXER_HPP
#define LEXER_HPP

#define FMT_HEADER_ONLY

#include "Compiler.hpp"
#include "Utility.hpp"
#include <cstdint>
#include <expected>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum class LexError : std::uint64_t {
    Eof = 0,
    EmptySource,
    UnexpectedCharacter,
    Max
};

enum class TokenType : std::uint64_t {
    Number = 0,
    Plus,
    KeywordOrIdentifier,
    Max
};

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
    [[nodiscard]] static auto lex(const std::shared_ptr<Compiler>& compiler)
      -> std::expected<std::vector<Token>, LexError>;

  private:
    explicit Lexer(const std::shared_ptr<Compiler>& compiler);

    [[nodiscard]] auto
      span(const std::size_t start, const std::size_t end) const -> Span;

    void error(const std::string& message, const Span& span);

    [[nodiscard]] auto eof() const -> bool;
    [[nodiscard]] auto peek() const -> std::expected<char, LexError>;
    [[nodiscard]] auto peek_ahead(const std::size_t offset) const
      -> std::expected<char, LexError>;
    [[nodiscard]] auto next() -> std::expected<Token, LexError>;

    [[nodiscard]] auto lex_keyword_identifier_or_number()
      -> std::expected<Token, LexError>;

    [[nodiscard]] auto lex_number() -> std::expected<Token, LexError>;

    std::shared_ptr<Compiler> m_compiler;
    std::string               m_source;
    std::size_t               m_cursor;
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

template<>
struct fmt::formatter<LexError> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const LexError& error, FormatContext& ctx) {
        static_assert(
          std::to_underlying(LexError::Max) == 3,
          "[INTERNAL ERROR] fmt::formatter<LexError> requires to handle all "
          "enum variants"
        );

        const auto enum_to_str = [](const LexError& error) {
            switch (error) {
                case LexError::Eof: {
                    return "LexError::Eof";
                }
                case LexError::EmptySource: {
                    return "LexError::EmptySource";
                }
                case LexError::UnexpectedCharacter: {
                    return "LexError::UnexpectedCharacter";
                }
                default: {
                    return "Unknown Lex Error";
                }
            }
        }();

        return fmt::format_to(ctx.out(), "{}", enum_to_str(error));
    }
};

#endif // LEXER_HPP
