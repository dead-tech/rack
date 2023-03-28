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
    [[nodiscard]] static auto create(const std::string& target)
      -> std::shared_ptr<Compiler>;

    [[nodiscard]] auto target() const -> std::string;
    [[nodiscard]] auto errors() const -> std::vector<RackError>;
    [[nodiscard]] auto file_contents() const -> std::string;

    void push_error(const RackError& error);
    void print_errors() const;

  private:
    explicit Compiler(std::string target);

    std::string            m_target;
    std::vector<RackError> m_errors;
    mutable std::string    m_file_contents;
};

#endif // COMPILER_HPP
