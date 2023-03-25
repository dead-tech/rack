#ifndef ERROR_HPP
#define ERROR_HPP

#include "Utility.hpp"
#include <dtslib/filesystem.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <string>

// TODO: Maybe later implement the ability to provide a hint to the error
//       e.g: std::optional<std::string> error;
struct RackError {
    std::string message;
    Span        span;
};

[[nodiscard]] auto compute_line_spans(const std::string& file_contents)
  -> std::vector<Span>;

void print_error(const RackError& error, const std::string& file_contents);

#endif // ERROR_HPP
