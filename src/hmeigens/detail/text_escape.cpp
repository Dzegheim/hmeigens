#include "hmeigens/detail/text_escape.hpp"

#include <string>
#include <string_view>
#include <format>
#include <locale>         // For std::isprint, std::locale

namespace hmeigens::detail {
    
    // Inside there is a cast because std::isprint has two overloads. This code uses the one in <locale>.
    // According to https://en.cppreference.com/cpp/locale/isprint it needs a std::ctype.
    bool isDisplayable (unsigned char toCheck) {
        return toCheck >= 0x80 || std::isprint(static_cast<char>(toCheck), std::locale::classic());
    }

    std::string escape (std::string_view toEscape) {
        std::string escaped;
        escaped.reserve(toEscape.size());
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
}

