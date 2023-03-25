#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Error.hpp"
#include <string>
#include <vector>

class Compiler {
  public:
    [[nodiscard]] static auto create() -> Compiler;

    [[nodiscard]] auto errors() const -> std::vector<RackError>;

    void push_error(const RackError& error);

  private:
    Compiler();

    std::vector<RackError> m_errors;
};

#endif // COMPILER_HPP
