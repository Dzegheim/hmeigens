#include "hmeigens/complex_parse.hpp"

#include <string>
#include <string_view>
#include <regex>
#include <charconv>       // For std::from_chars
#include <system_error>   // For std::errc
#include <stdexcept>      // For std::invalid_argument
#include <format>
#include <locale>         // For std::isprint, std::locale

namespace hmeigens {

    namespace detail {
        // This function checks if a character is printable in the way that matters to this code.
        // The locale used is the std::locale::classic(), as it is the only one that will be used inside the code, so that the same input produces the same output independently of the user's locale settings.
        // From 128 onward there is no need to check here, those are not ASCII.
        // Should anyone ever input a non-ASCII character, like an emoji (why would they?), escaping its components would display it as a mumbojumbo of octals, and the user would have a hard time reading the error message.
        // The chars before 128 are checked by std::isprint.
        // The parameter is unsigned char because char can be signed or unsigned depending on the compiler, so performing these types of checks on it is dangerous.
        // Inside there is a cast because std::isprint has two overloads: this code uses the one in <locale>.
        // According to https://en.cppreference.com/cpp/locale/isprint it needs a std::ctype.
        // The argument must be a char, passing an unsigned one compiles and links but throws std::bad_cast at runtime.
        bool isDisplayable (unsigned char toCheck) {
            return toCheck >= 0x80 || std::isprint(static_cast<char>(toCheck), std::locale::classic());
        }

        // This function renders arbitrary input as text that is safe to put in a message by escaping problematic characters.
        // Trusting the user to sanitize input is always a mistake.
        // Since this code wants to provide users with the offending text when an input is invalid, if characters such as \0 are not escaped, they may break the message.
        // Escapes use three octal digits rather than hexadecimal escapes.
        // From https://en.cppreference.com/cpp/language/escape:
        // "Hexadecimal escape sequences have no length limit and terminate at the first character that is not a valid hexadecimal digit. If the value represented by a single hexadecimal escape sequence does not fit the range of values represented by the character type used in this string literal (char, char8_t, (since C++20)char16_t, char32_t, (since C++11)or wchar_t), the result is unspecified."
        std::string escape (std::string_view toEscape) {
            std::string escaped;
            escaped.reserve(toEscape.size());
            // To see why unsigned char see isDisplayable's comment above.
            for (const unsigned char character : toEscape) {
                // Backslash and quotes are escaped or the rendering could be ambiguous.
                if (character == '\\') {
                    escaped += R"(\\)";
                }
                else if (character == '"') {
                    escaped += R"(\")";
                }
                // For legibility reasons the following characters are not rendered as octals.
                // Since these letters are not octal digits they don't merge with other digits that follow them.
                // There is no \0 because instead a \0 followed by a 1 would be \01 and not be the input.
                // There is no apostrophe because strings in this code are formatted with qoutes as delimiters
                else if (character == '\n') {
                    escaped += R"(\n)";
                }
                else if (character == '\r') {
                    escaped += R"(\r)";
                }
                else if (character == '\t') {
                    escaped += R"(\t)";
                }
                // Check if the character is displayable.
                else if (isDisplayable(character)) {
                    escaped += static_cast<char>(character);
                }
                // If it is not, turn it into octal.
                else {
                    escaped += std::format("\\{:03o}", character);
                }
            }
            return escaped;
        }

        // This function converts the submatch to a corresponding hmeigens::Scalar.
        // Since std::from_chars has no locale, there is no risk that someone changing the decimal separator affects the way the code reads the numbers.
        // Note from https://en.cppreference.com/cpp/utility/from_chars: "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)".
        // The case of a leading '+' is accepted in the input, and is handled manually in this function.
        // The fullInput parameter is the entire string containing submatch, and is only used to provide accurate diagnostic messages in case of errors.
        hmeigens::Scalar toScalar (const std::csub_match& submatch, std::string_view fullInput) {
            // The iterators of cmatch are const char*.
            // The second is the past-the-end location.
            const char* first = submatch.first;
            const char* last  = submatch.second;
            // Check that the match is not empty, and that the first character is a '+'.
            // If it is a '+', go forward one.
            if (first != last && *first == '+') {
               ++first;
            }
            hmeigens::Scalar value{};
            // The names for ptr and ec are the ones from the standard.
            // In the structured binding ptr points past-the-end if successful, or to the first non matching character, ec is an error code.
            // The value is untouched in case of errors.
            const auto [ptr, ec] = std::from_chars(first, last, value);
            // The value was read correctly only if ptr is last, and if ec is default.
            if (ec != std::errc{} || ptr != last) {
                // If there is an error, throw with the offending text.
                // The input is already checked by hmeigens::parseComplex, so a std::errc::invalid_argument should never happen.
                // What can happen here is std::errc::result_out_of_range if the passed value does not fit in the chosen Scalar type.
                if (ec == std::errc::result_out_of_range) {
                    throw ParseError{
                        // Full offending text.
                        fullInput,
                        // Error message constructed to signify out of range.
                        // The piece of offending text that caused the problem is submatch.str().
                        std::format("Input {0} is out of range for Scalar type {1}.", submatch.str(), hmeigens::scalarType)
                    };
                }
                // This should never be reached because of previous checks, but better safe than sorry.
                throw ParseError{
                    // Full offending text.
                    fullInput,
                    std::format("The text {0} caused the problem. This is a bug in HMEigenS, not in your input. Please report it on GitHub with the text you entered. Thank you.", submatch.str())};
            }
            return value;
        }
    }

    hmeigens::Complex parseComplex(std::string_view input) {
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
                // See toScalar about sign problems when the input starts with '+'.
                hmeigens::detail::toScalar(match[1], input),
                // Null imaginary part.
                hmeigens::Scalar {0.0}
            };
        }
        // Ordered pair case.
        if (std::regex_match(first, last, match, orderedPair)) {
            return hmeigens::Complex{
                // Matched real part, including sign.
                // See toScalar about sign problems when the input starts with '+'.
                hmeigens::detail::toScalar(match[1], input),
                // Matched imaginary part, including sign.
                hmeigens::detail::toScalar(match[2], input)
            };
        }
        // Algebraic expression case.
        if (std::regex_match(first, last, match, algebraicForm)) {
            // If a real part is matched, it is used.
            // If not matched, it is null.
            const hmeigens::Scalar realPart = match[1].matched ? hmeigens::detail::toScalar(match[1], input) : hmeigens::Scalar{0.0};
            // If there is a sign, it is used.
            // If there is no sign, it is a '+'.
            const hmeigens::Scalar imagSign = (match[2].matched && *match[2].first == '-') ? hmeigens::Scalar{-1.0} : hmeigens::Scalar{1.0};
            // If there is a magnitude, it is used.
            // If there is no magnitude, it is 1.0.
            const hmeigens::Scalar imagMagnitude = match[3].matched ? hmeigens::detail::toScalar(match[3], input) : hmeigens::Scalar{1.0};
            return hmeigens::Complex{realPart, imagSign * imagMagnitude};
        }
        // If nothing matches an exception is thrown.
        throw hmeigens::ParseError{input};
    }

    ParseError::ParseError(std::string_view input) : std::invalid_argument(std::format("Unable to parse a complex number from \"{0}\".", detail::escape(input))) {}

    ParseError::ParseError(std::string_view input, std::string_view errorMessage) : std::invalid_argument(std::format("Unable to parse a complex number from \"{0}\".\n---> {1}", input, detail::escape(errorMessage))) {}
}
