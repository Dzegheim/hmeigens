/// @file
/// @brief Parser for complex numbers from text.
#ifndef HMEIGENS_COMPLEX_PARSE_HPP
#define HMEIGENS_COMPLEX_PARSE_HPP

#include "hmeigens/scalar.hpp"

#include <string_view>
#include <stdexcept>     // For std::invalid_argument

namespace hmeigens {
    /// @brief Exception thrown when text cannot be parsed as a complex number.
    ///
    /// Inherits from `std::invalid_argument`, so it can be caught as that if it's not caught specifically as `ParseError`.
    class ParseError : public std::invalid_argument {
        public:
        /// @brief One-argument constructor: parsed text **cannot** be interpreted into a complex number.
        ///
        /// @param input The offending text that was not successfully parsed.
        explicit ParseError(std::string_view input);
        /// @brief Two-argument constructor: parsed text **can** be read into a complex number, but something about it failed, specified by the second argument.
        ///
        /// @param input The offending text that was not successfully parsed.
        /// @param errorMessage An additional text supplied to provide additional information on the type of failure the parser experienced.
        explicit ParseError(std::string_view input, std::string_view errorMessage);
    };

    /// @brief Converts text into a corresponding complex number.
    ///
    /// Accepted forms: 
    ///
    /// | Form      | Example        |  (Real, Imaginary)   |
    /// |-----------|----------------|----------------------|
    /// |   `a`     |      `1.2`     |       (1.2,0)        |
    /// |  `a+bi`   |     `1-2i`     |       (1,-2)         |
    /// |   `bi`    |     `-3.i`     |       (0,-3)         |
    /// |   `i`     |      `+i`      |       (0,1)          |
    /// |  `a,b`    |     `1,-2.3`   |       (1,-2.3)       |
    /// In addition:
    /// - numbers may start or end with a `.`: `2.` is perfectly valid;
    /// - leading `+` and `-` are accepted: `+2` is perfectly valid;
    /// - accepted letters are case insensitive: `2e2-.3E3I` is perfectly valid;
    /// - leading or trailing whitespace is rejected: ` 2` is invalid;
    /// - suffixes, digit separators, and hex are rejected: `2.1f` is invalid.
    /// @todo Trim whitespace when file input is supported.
    /// @todo Accept suffixes, digit separators, and hex.
    /// @param input The text to read.
    /// @return An `hmeigens::Complex` corresponding to the `input`.
    /// @throws ParseError if `input` cannot be read into a complex number.
    [[nodiscard]] hmeigens::Complex parseComplex(std::string_view input);
}

#endif
