#include "Utility.hpp"

auto Span::create(const std::size_t start, const std::size_t end) -> Span {
    return { start, end };
}

auto Span::start() const -> std::size_t { return this->m_start; }

auto Span::end() const -> std::size_t { return this->m_end; }

Span::Span(const std::size_t start, const std::size_t end)
  : m_start{ start },
    m_end{ end } {}
