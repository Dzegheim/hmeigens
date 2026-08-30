#include "hmeigens/detail/text_escape.hpp"

#include <string>
#include <string_view>
using namespace std::string_view_literals;      // For ""sv

#include <catch2/catch_test_macros.hpp>

// This is a helper function to verify that the excaped text is the same as the expected one.
static void checkEscape (std::string_view toEscape, std::string_view expectedText) {
    CAPTURE(expectedText);
    CHECK(hmeigens::detail::escape(toEscape) == expectedText);
}

TEST_CASE("Displayable chars test: special characters are correctly identified.", "[text_escape]") {
    // GIVEN ASCII and Non-ASCII characters
    // WHEN  they are passed to hmeigens::detail::isDisplayable
    // THEN  the function correctly reports on their displayability in std::locale::classic()
    //
    // ASCII.
    // NUL byte.
    CHECK_FALSE(hmeigens::detail::isDisplayable('\0'));
    // Last control character until DEL.
    CHECK_FALSE(hmeigens::detail::isDisplayable(31));
    // First printable ASCII - space.
    CHECK (hmeigens::detail::isDisplayable(32));
    // Last printable ASCII - tilde.
    CHECK (hmeigens::detail::isDisplayable(126));
    // DEL
    CHECK_FALSE(hmeigens::detail::isDisplayable(127));
    // Non-ASCII - these cases verify the non-std::isprint's branch.
    // First non-ASCII.
    CHECK (hmeigens::detail::isDisplayable(128));
    // Last unsigned char.
    CHECK (hmeigens::detail::isDisplayable(255));
}
// ----------------------------------------------------------
// The tests below assume hmeigens::detail::isDisplayable is trusted in its output, as it is tested independently above.
// ----------------------------------------------------------

TEST_CASE ("Escape test: regular printable text is unaffected.", "[text_escape]") {
    // GIVEN regularly printable text
    // WHEN  is it ran through hmeigens::detail::escape
    // THEN  it is completely unaffected
    checkEscape("", "");
    checkEscape(" ", " ");
    checkEscape("The quick brown fox jumped over the lazy dog.", "The quick brown fox jumped over the lazy dog.");
    // Non-ASCII.
    checkEscape("|!£$%&/()=-_+*àèéìòù^~'`.,:;[]{}", "|!£$%&/()=-_+*àèéìòù^~'`.,:;[]{}");
    checkEscape("1.🤬i", "1.🤬i");
    checkEscape("悪い入力", "悪い入力");
    checkEscape("𓃥𓃠𓆉𓆏𓃯𓃱𓃰", "𓃥𓃠𓆉𓆏𓃯𓃱𓃰");
    checkEscape("𓂀𓂀𓂀𓋹𓁈𓃠𓆃☥𓅓𓆣", "𓂀𓂀𓂀𓋹𓁈𓃠𓆃☥𓅓𓆣");
}

TEST_CASE ("Escape test: control characters are escaped.", "[text_escape]") {
    // GIVEN text containing control characters
    // WHEN  is it ran through hmeigens::detail::escape
    // THEN  the characters are correctly escaped
    //
    // These three are printed in their "readable" form.
    checkEscape("\n"sv, R"(\n)");
    checkEscape("\r"sv, R"(\r)");
    checkEscape("\t"sv, R"(\t)");
    // Other unprintables are rendered as three digit octals.
    checkEscape("\0"sv, R"(\000)");
    checkEscape("\a"sv, R"(\007)");
    checkEscape("\177"sv, R"(\177)");
    // Strings containing the control characters in the middle.
    checkEscape("a\nb"sv, R"(a\nb)");
    checkEscape("You\0shouldn't\atype\rlike\tthis."sv, R"(You\000shouldn't\007type\rlike\tthis.)");
}

TEST_CASE ("Escape test: backslash and quotes are escaped.", "[text_escape]") {
    // GIVEN text containing backslashes and quotes
    // WHEN  it is ran through hmeigens::detail::escape
    // THEN  the characters are correctly escaped
    checkEscape("\\"sv, R"(\\)");
    checkEscape(R"(\)", R"(\\)");
    checkEscape("\""sv, R"(\")");
    checkEscape(R"(")", R"(\")");
    checkEscape(R"("3.3\")", R"(\"3.3\\\")");
}

TEST_CASE ("Escape test: Non-ASCII and control characters can live in the same string and only controls are escaped.", "[text_escape]") {
    // GIVEN text containing both non-ASCII characters and control characters
    // WHEN  it is ran through hmeigens::detail::escape
    // THEN  non-ASCII are unaffected, the control characters are escaped
    checkEscape("\n🤬"sv, R"(\n🤬)");
    checkEscape("悪い\0入力"sv, R"(悪い\000入力)");
    checkEscape("𓃥𓃠\t𓆉𓆏𓃯𓃱𓃰"sv, R"(𓃥𓃠\t𓆉𓆏𓃯𓃱𓃰)");
}
