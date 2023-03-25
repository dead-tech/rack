#ifndef UTILITY_HPP
#define UTILITY_HPP

#define FMT_HEADER_ONLY

#include <cstdint>
#include <fmt/format.h>

class Span {
  public:
    [[nodiscard]] static auto create(
      const std::string& file_id,
      const std::size_t  start,
      const std::size_t  end
    ) -> Span;

    [[nodiscard]] auto file_id() const -> std::string;
    [[nodiscard]] auto start() const -> std::size_t;
    [[nodiscard]] auto end() const -> std::size_t;

  private:
    Span(std::string file_id, const std::size_t start, const std::size_t end);

    std::string m_file_id;
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
          ctx.out(),
          "Span {{ file_id: {}, start: {}, end: {} }}",
          span.file_id(),
          span.start(),
          span.end()
        );
    }
};

#endif // UTILITY_HPP
