#ifndef ERROR_HPP
#define ERROR_HPP

#include "Utility.hpp"
#include <string>

// TODO: Maybe later implement the ability to provide a hint to the error
//       e.g: std::optional<std::string> error;
struct RackError {
    std::string message;
    Span        span;
};

#endif // ERROR_HPP
