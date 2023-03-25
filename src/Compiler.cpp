#include "Compiler.hpp"

auto Compiler::create(const std::string& target) -> Compiler {
    return Compiler(target);
}

Compiler::Compiler(std::string target)
  : m_target{ std::move(target) },
    m_errors{ {} } {}

auto Compiler::target() const -> std::string { return this->m_target; }

auto Compiler::errors() const -> std::vector<RackError> {
    return this->m_errors;
}

auto Compiler::file_contents() const -> std::string {
    if (this->m_file_contents.empty()) {
        const auto contents = dts::read_file<std::string>(this->m_target);
        if (!contents.has_value()) {
            fmt::print(stderr, fmt::fg(fmt::color::red), "error");
            fmt::print(stderr, fmt::emphasis::bold, ": empty file\n");
        }
        this->m_file_contents = contents.value();
    }

    return this->m_file_contents;
}

void Compiler::push_error(const RackError& error) {
    this->m_errors.push_back(error);
}
