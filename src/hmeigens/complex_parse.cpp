#include "hmeigens/complex_parse.hpp"

#include <string>
#include <regex>
#include <charconv>       // For std::from_chars
#include <system_error>   // For std::errc

namespace hmeigens {

    namespace detail {

        // This function actually converts the matched submatch to a corresponding hmeigens::Scalar.
        // Using std::from_chars has no locale, so a different decimal separator does not break anything.
        // Note from https://en.cppreference.com/cpp/utility/from_chars: "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)".
        // The case of a leading '+' is accepted in the input, and is handled manually in this function.
        hmeigens::Scalar toScalar (const std::csub_match& submatch) {
            // The iterators of cmatch are const char*.
            // The second is the past-the-end location.
            const char* first = submatch.first;
            const char* last  = submatch.second;
            // Check that the match is not empty, and that the first character is not a '+'.
            // If it is a '+', go forward one.
            if (first != last && *first == '+') {
               ++first;
             }
            hmeigens::Scalar value{};
            // The names for ptr and ec are the ones from the standard.
            // In the structured binding ptr points to the end if successful, or to the first non matching character, ec is an error code.
            // The value is untouched in case of errors.
            const auto [ptr, ec] = std::from_chars(first, last, value);
            // The value was read correctly only if ptr is last, and if ec is an empty error code.
            if (ec != std::errc{} || ptr != last) {
                // If there is an error, throw the offending text.
                // This should never happen here because hmeigens::parseComplex already checks the input, but better safe than sorry.
                throw ParseError{
                    std::string_view{
                    // Not first, in case it was increased because of a leading '+'.
                    submatch.first,
                    // Length of the offending text.
                    static_cast<std::size_t>(last - submatch.first)}
                };
            }
            return value;
        }
    }

    hmeigens::Complex parseComplex(std::string_view input) {
        // String for a generic number with optional exponential part.
        // Structure:
        // (?:\d+(?:\.\d+)?|\.\d+)  123 OR 123.45 OR .45 
        // (?:[eE][\+\-]?\d+)?      Optional exponent
        static const std::string unsignedNumber {R"((?:\d+(?:\.\d+)?|\.\d+)(?:[eE][\+\-]?\d+)?)"};
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
                // See toScalar about sign problems when the input starts with '+'.
                hmeigens::detail::toScalar(match[1]),
                // Null imaginary part.
                hmeigens::Scalar {0.0}
            };
        }
        // Ordered pair case.
        if (std::regex_match(first, last, match, orderedPair)) {
            return hmeigens::Complex{
                // Matched real part, including sign.
                // See toScalar about sign problems when the input starts with '+'.
                hmeigens::detail::toScalar(match[1]),
                // Matched imaginary part, including sign.
                hmeigens::detail::toScalar(match[2])
            };
        }
        // Algebraic expression case.
        if (std::regex_match(first, last, match, algebraicForm)) {
            // If a real part is matched, it is used.
            // If not matched, it is null.
            const hmeigens::Scalar realPart = match[1].matched ? hmeigens::detail::toScalar(match[1]) : hmeigens::Scalar{0.0};
            // If there is a sign, it is used.
            // If there is no sign, it is a '+'.
            const hmeigens::Scalar imagSign = (match[2].matched && *match[2].first == '-') ? hmeigens::Scalar{-1.0} : hmeigens::Scalar{1.0};
            // If there is a magnitude, it is used.
            // If there is no magnitude, it is 1.0.
            const hmeigens::Scalar imagMagnitude = match[3].matched ? hmeigens::detail::toScalar(match[3]) : hmeigens::Scalar{1.0};
            return hmeigens::Complex{realPart, imagSign * imagMagnitude};
        }
        // If nothing matches an exception is thrown.
        throw hmeigens::ParseError{input};
    }
}