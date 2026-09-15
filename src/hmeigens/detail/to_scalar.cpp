#include "hmeigens/detail/to_scalar.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/complex_parse.hpp"
#include "hmeigens/detail/text_escape.hpp"

#include <string>
#include <string_view>
#include <charconv>       // For std::from_chars
#include <system_error>   // For std::errc
#include <format>

// Note from https://en.cppreference.com/cpp/utility/from_chars: "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)".
// The case of a leading '+' is accepted in the input, and is handled manually in this function.
[[nodiscard]] hmeigens::Scalar hmeigens::detail::toScalar (std::string_view toConvert, std::string_view fullInput) {
    // Stored here because if a leading "+" is removed, the user can still get the original text meant for conversion in an error message.
    const std::string_view originalToConvert = toConvert;
    // Check that toConvert is not empty, and that the first character is a '+'.
    // If it is a '+', drop it.
    if (!toConvert.empty() && toConvert.front() == '+') {
        toConvert.remove_prefix(1);
    }
    // Get the start and end of toConvert.
    const char* first = toConvert.data();
    const char* last  = toConvert.data()+toConvert.size();
    hmeigens::Scalar value{};
    // The names for ptr and ec are the ones from the standard.
    // In the structured binding ptr points past-the-end if successful, or to the first non matching character, ec is an error code.
    // The value is untouched in case of errors.
    const auto [ptr, ec] = std::from_chars(first, last, value);
    // The value was read correctly only if ptr is last, and if ec is default.
    if (ec != std::errc{} || ptr != last) {
        // If there is an error, throw with the offending text.
        // The input is already checked by hmeigens::parseComplex, so a std::errc::invalid_argument should never happen.
        // What can happen here is std::errc::result_out_of_range if the passed value does not fit in the chosen hmeigens::Scalar type.
        if (ec == std::errc::result_out_of_range) {
            throw hmeigens::ParseError{
                // Full offending text.
                fullInput,
                // Error message constructed to signify out of range.
                // The piece of offending text that caused the problem is originalToConvert.
                std::format("Input {0} is out of range for Scalar type {1}.", hmeigens::detail::escape(originalToConvert), hmeigens::scalarType)
            };
        }
        // This should never be reached because of previous checks, but better safe than sorry.
        throw hmeigens::ParseError{
            // Full offending text.
            fullInput,
            std::format("The text {0} caused the problem. This is a bug in HMEigenS, not in your input. Please report it on GitHub with the text you entered. Thank you.", hmeigens::detail::escape(originalToConvert))};
    }
    return value;
}
