#include "Error.hpp"

[[nodiscard]] auto compute_line_spans(const std::string& file_contents)
  -> std::vector<Span> {
    std::vector<Span> line_spans;

    std::size_t start = 0;
    for (std::size_t i = 0; i < file_contents.size(); ++i) {
        if (file_contents[i] == '\n') {
            line_spans.push_back(Span::create("", start, ++i));
            start = i + 1;
        }
    }

    return line_spans;
}

void print_error(const RackError& error, const std::string& file_contents) {
    if (file_contents.empty()) { return; }

    fmt::print(stderr, fmt::fg(fmt::color::red), "error");
    fmt::print(stderr, fmt::emphasis::bold, ": {}\n", error.message);

    // Find in which line is present the error span
    std::size_t error_line_index  = 0;
    std::size_t error_line_number = 0;
    const auto  line_spans        = compute_line_spans(file_contents);
    for (std::size_t line_index = 0; line_index < line_spans.size();
         ++line_index) {
        const auto& line_span = line_spans[line_index];
        if (error.span.start() >= line_span.start() && error.span.start() <= line_span.end()) {
            error_line_index = line_index;
        }

        if (error.span.end() >= line_span.start() && error.span.end() <= line_span.end()) {
            error_line_number = line_index + 1;
        }
    }

    fmt::println(
      stderr,
      " --> {}:{}:{}",
      error.span.file_id(),
      error_line_number,
      error.span.start()
    );
    fmt::println(stderr, "  |");
    fmt::print(stderr, "  {} \t", error_line_number);

    // Print error line contents
    const auto& error_line_span     = line_spans[error_line_index];
    const auto  error_line_contents = file_contents.substr(
      error_line_span.start(), error_line_span.start() + error_line_span.end()
    );
    fmt::print(stderr, "{}", error_line_contents);

    // Print '^^^^' below span and error message next
    const auto spaces =
      std::string(error.span.start() - error_line_span.start(), ' ');
    const auto carets = std::string(error.span.end() - error.span.start(), '^');
    fmt::print(
      stderr,
      fmt::fg(fmt::color::red),
      "  |    \t{}{} {}\n",
      spaces,
      carets,
      error.message
    );
    fmt::println(stderr, "  |");
}
