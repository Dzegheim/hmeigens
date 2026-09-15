#include "hmeigens/complex_parse.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/detail/text_escape.hpp"
#include "hmeigens/detail/to_scalar.hpp"

#include <string>
#include <string_view>
#include <regex>
#include <stdexcept>      // For std::invalid_argument
#include <format>

hmeigens::Complex hmeigens::parseComplex(std::string_view input) {
    // String for a generic number with optional exponential part.
    // Structure:
    // (?:\d+(?:\.\d*)?|\.\d+)  123 OR 123.45 OR .45 OR 123.
    // (?:[eE][\+\-]?\d+)?      Optional exponent
    static const std::string unsignedNumber {R"((?:\d+(?:\.\d*)?|\.\d+)(?:[eE][\+\-]?\d+)?)"};
    // String for the sign of a number
    static const std::string sign {R"([\+\-])"};
    // String for an (optionally) signed number.
    // This is just to avoid repeating sign + "?" + unsignedNumber many times below.
    static const std::string signedNumber {sign + "?" + unsignedNumber};
    // This regex matches a real number, with a start and an end of string limitators.
    static const std::regex realNumber {"^(" + signedNumber + ")$"};
    //This regex matches an ordered pair complex number in the form a,b, with a and b both signed numbers.
    static const std::regex orderedPair {"^(" + signedNumber + "),(" + signedNumber + ")$"};
    //This regex matches the algebraic form of a complex number.
    //Structure
    // ^                   Start of the string
    // (                   Group 1: real part
    // signedNumber
    // (?![\.\diI])        Negative lookahead: needed to prevent 7i being read as 7+i
    // )?                  Group 1 is optional
    // (sign)?             Group 2: sign of the imaginary part, optional
    // (                   Group 3: magnitude of the imaginary part
    // unsignedNumber
    // )?                  Group 3 is optional, otherwise "i" would not be a valid input
    // [iI]                Mandatory, if this is missing the regex matches the empty string and this regex doesn't need to match real numbers, already handled above
    // $                   End of the string
    static const std::regex algebraicForm {"^(" + signedNumber + R"((?![\.\diI]))?()" + sign +")?(" + unsignedNumber + ")?[iI]$"};

    // Get the start and end of the read data.
    const auto* first = input.data();
    const auto* last  = input.data() + input.size();
    
    // Using cmatch since the input is std::string_view.
    std::cmatch match;

    // Real number case.
    if (std::regex_match(first, last, match, realNumber)) {
        return hmeigens::Complex {
            // Matched real part, including sign.
            hmeigens::detail::toScalar(std::string_view{match[1].first, match[1].second}, input),
            // Null imaginary part.
            hmeigens::Scalar {0.0}
        };
    }
    // Ordered pair case.
    if (std::regex_match(first, last, match, orderedPair)) {
        return hmeigens::Complex{
            // Matched real part, including sign.
            hmeigens::detail::toScalar(std::string_view{match[1].first, match[1].second}, input),
            // Matched imaginary part, including sign.
            hmeigens::detail::toScalar(std::string_view{match[2].first, match[2].second}, input),
        };
    }
    // Algebraic expression case.
    if (std::regex_match(first, last, match, algebraicForm)) {
        // If a real part is matched, it is used.
        // If not matched, it is null.
        const hmeigens::Scalar realPart = match[1].matched ?
        hmeigens::detail::toScalar(std::string_view{match[1].first, match[1].second}, input) :
        hmeigens::Scalar{0.0};
        // If there is a sign, it is used.
        // If there is no sign, it is a '+'.
        const hmeigens::Scalar imagSign = (match[2].matched && *match[2].first == '-') ? hmeigens::Scalar{-1.0} : hmeigens::Scalar{1.0};
        // If there is a magnitude, it is used.
        // If there is no magnitude, it is 1.0.
        const hmeigens::Scalar imagMagnitude = match[3].matched
        ? hmeigens::detail::toScalar(std::string_view{match[3].first, match[3].second}, input) :
        hmeigens::Scalar{1.0};
        return hmeigens::Complex{realPart, imagSign * imagMagnitude};
    }
    // If nothing matches an exception is thrown.
    throw hmeigens::ParseError{input};
}

hmeigens::ParseError::ParseError(std::string_view input) : std::invalid_argument(std::format("Unable to parse a complex number from \"{0}\".", hmeigens::detail::escape(input))) {}

// The errorMessage needs no escaping, as it is built by internal functions that escape text on their own.
hmeigens::ParseError::ParseError(std::string_view input, std::string_view errorMessage) : std::invalid_argument(std::format("Unable to parse a complex number from \"{0}\".\n---> {1}", hmeigens::detail::escape(input), errorMessage)) {}
