#ifndef HMEIGENS_COMPLEX_PARSE_HPP
#define HMEIGENS_COMPLEX_PARSE_HPP

#include "hmeigens/scalar.hpp"

#include <stdexcept>     // For std::invalid_argument
#include <string_view>

namespace hmeigens {

    struct ParseError : public std::invalid_argument {
        
        explicit ParseError(std::string_view input) : std::invalid_argument("Unable to parse a complex number from \"" + std::string{input} + "\".") {}
    };

    [[nodiscard]] hmeigens::Complex parseComplex(std::string_view input);
}

#endif