#ifndef COMPILER_HPP
#define COMPILER_HPP

#define FMT_HEADER_ONLY

#include "Error.hpp"
#include <fmt/color.h>
#include <fmt/format.h>
#include <string>
#include <vector>

class Compiler {
  public:
    [[nodiscard]] static auto
      create(const std::string& target, const std::string& output)
        -> std::shared_ptr<Compiler>;

    [[nodiscard]] auto target() const -> std::string;
    [[nodiscard]] auto errors() const -> std::vector<RackError>;
    [[nodiscard]] auto file_contents() const -> std::string;
    [[nodiscard]] auto output() const -> std::string;

    [[nodiscard]] auto has_errors() const -> bool;

    void push_error(const RackError& error);
    void print_errors() const;

  private:
    Compiler(std::string target, std::string output);

    std::string            m_target;
    std::vector<RackError> m_errors;
    mutable std::string    m_file_contents;
    std::string            m_output;
};

#endif // COMPILER_HPP
