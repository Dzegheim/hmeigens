// Source file for all functions in text_escape.hpp.
// All functions are fully documented at their declarations.
// Definitions (hmeigens::detail):
//     - isDisplayable(unsigned char);
//     - escape(std::string_view).
#include "hmeigens/detail/text_escape.hpp"

#include <string>
#include <string_view>
#include <format>
#include <locale>         // For std::isprint, std::locale

bool hmeigens::detail::isDisplayable(unsigned char toCheck) {
    // The chars before 128 are checked by std::isprint.
    // According to https://en.cppreference.com/cpp/locale/isprint it needs an std::ctype.
    // The argument must be a char, passing an unsigned one compiles and links but throws std::bad_cast at runtime.
    // The first non-ASCII is 0x80 (128 decimal, 0200 octal), and from it onward there is no need to check here.
    return toCheck >= 0x80 or std::isprint(static_cast<char>(toCheck), std::locale::classic());
}

std::string hmeigens::detail::escape(std::string_view toEscape) {
    std::string escaped;
    escaped.reserve(toEscape.size());
    for (const auto c : toEscape) {
        // This cast makes the conversion to unsigned explicit, so that the compiler does not complain.
        const auto character = static_cast<unsigned char>(c);
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
        // There is no apostrophe because strings in this code are formatted with quotes as delimiters.
        else if (character == '\n') {
            escaped += R"(\n)";
        }
        else if (character == '\r') {
            escaped += R"(\r)";
        }
        else if (character == '\t') {
            escaped += R"(\t)";
        }
        // Check if the character is displayable, and if it is not, turn it into octal.
        else if (hmeigens::detail::isDisplayable(character)) {
            escaped += static_cast<char>(character);
        }
        else {
            escaped += std::format("\\{:03o}", character);
        }
    }
    return escaped;
}
