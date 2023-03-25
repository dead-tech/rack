#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdint>
#include <fmt/format.h>

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

// {fmt} Custom Formatters
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

#endif // UTILITY_HPP
