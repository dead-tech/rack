#include "Compiler.hpp"

auto Compiler::create(const std::string& target, const std::string& output)
  -> std::shared_ptr<Compiler> {
    return std::make_shared<Compiler>(Compiler(target, output));
}

Compiler::Compiler(std::string target, std::string output)
  : m_target{ std::move(target) },
    m_errors{ {} },
    m_output{ std::move(output) } {}

auto Compiler::target() const -> std::string { return this->m_target; }

auto Compiler::errors() const -> std::vector<RackError> {
    return this->m_errors;
}

auto Compiler::has_errors() const -> bool { return !this->m_errors.empty(); }

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

auto Compiler::output() const -> std::string { return this->m_output; }

void Compiler::push_error(const RackError& error) {
    this->m_errors.push_back(error);
}

void Compiler::print_errors() const {
    for (const auto& error : this->m_errors) {
        print_error(error, this->file_contents());
    }
    fmt::print(stderr, fmt::fg(fmt::color::red), "error");
    fmt::print(
      stderr, fmt::emphasis::bold, ": aborting due to previous error(s)\n"
    );
}
