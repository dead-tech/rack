#include "Utility.hpp"

auto Span::create(
  const std::string& file_id,
  const std::size_t  start,
  const std::size_t  end
) -> Span {
    return { file_id, start, end };
}

auto Span::file_id() const -> std::string { return this->m_file_id; }

auto Span::start() const -> std::size_t { return this->m_start; }

auto Span::end() const -> std::size_t { return this->m_end; }

Span::Span(std::string file_id, const std::size_t start, const std::size_t end)
  : m_file_id{ std::move(file_id) },
    m_start{ start },
    m_end{ end } {}
