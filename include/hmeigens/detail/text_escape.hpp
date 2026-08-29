// File for functions used to process input strings.
//
// Input may contain control characters that may break diagnostics messages.

#ifndef HMEIGENS_DETAIL_TEXT_ESCAPE_HPP
#define HMEIGENS_DETAIL_TEXT_ESCAPE_HPP

#include <string>
#include <string_view>

namespace hmeigens::detail {
    // This function checks if a character is printable in the way that matters to this code.
    // The locale used is the std::locale::classic(), as it is the only one that will be used inside the code, so that the same input produces the same output independently of the user's locale settings.
    // From 128 onward there is no need to check here, those are not ASCII.
    // Should anyone ever input a non-ASCII character, like an emoji (why would they?), escaping its components would display it as a mumbojumbo of octals, and the user would have a hard time reading the error message.
    // The chars before 128 are checked by std::isprint.
    // The parameter is unsigned char because char can be signed or unsigned depending on the compiler, so performing these types of checks on it is dangerous.
    // The argument must be a char, passing an unsigned one compiles and links but throws std::bad_cast at runtime.
    [[nodiscard]] bool isDisplayable (unsigned char toCheck);

    // This function renders arbitrary input as text that is safe to put in a message by escaping problematic characters.
    // Trusting the user to sanitize input is always a mistake.
    // Since this code wants to provide users with the offending text when an input is invalid, if characters such as \0 are not escaped, they may break the message.
    // Escapes use three octal digits rather than hexadecimal escapes.
    // From https://en.cppreference.com/cpp/language/escape:
    // "Hexadecimal escape sequences have no length limit and terminate at the first character that is not a valid hexadecimal digit. If the value represented by a single hexadecimal escape sequence does not fit the range of values represented by the character type used in this string literal (char, char8_t, (since C++20)char16_t, char32_t, (since C++11)or wchar_t), the result is unspecified."
    [[nodiscard]] std::string escape (std::string_view toEscape);
}

#endif
