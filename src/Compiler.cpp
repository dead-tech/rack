#include "Compiler.hpp"

auto Compiler::create() -> Compiler { return {}; }

Compiler::Compiler()
  : m_errors{ {} } {}

auto Compiler::errors() const -> std::vector<RackError> {
    return this->m_errors;
}

void Compiler::push_error(const RackError& error) {
    this->m_errors.push_back(error);
}
