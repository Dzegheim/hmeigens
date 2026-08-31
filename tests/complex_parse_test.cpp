#include "hmeigens/constants.hpp"
#include "hmeigens/complex_parse.hpp"
#include "hmeigens/detail/text_escape.hpp"

#include <string_view>
using namespace std::string_view_literals;      // For ""sv

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

// This is a helper function to test whether the text is parsed correctly.
// Apparently Catch2's floating-point matchers always compare in double (see catch_matchers_floating_point.hpp).
// Source: https://github.com/catchorg/Catch2/blob/v3.15.3/docs/comparing-floating-point-numbers.md#withinabs
static void checkParse (std::string_view testString, double expectedReal, double expectedImag) {
    // CAPTURE prints the captured value at the time of capture if the test doesn't pass.
    CAPTURE(testString);
    const auto z = hmeigens::parseComplex(testString);
    // This function only checks against doubles.
    // It's what Catch2 would do anyway, so here it's explicit.
    const auto real = static_cast<double>(z.real());
    const auto imag = static_cast<double>(z.imag());
    constexpr double dblParseTolerance = static_cast<double>(hmeigens::parseTolerance);
    CHECK_THAT(
        real,
        Catch::Matchers::WithinAbs(expectedReal, dblParseTolerance)
        ||
        Catch::Matchers::WithinRel(expectedReal, dblParseTolerance)
    );
    CHECK_THAT(
        imag,
        Catch::Matchers::WithinAbs(expectedImag, dblParseTolerance)
        ||
        Catch::Matchers::WithinRel(expectedImag, dblParseTolerance)
    );
}

// This is a helper function to test whether text that is not supposed to be parsed:
// - throws;
// - throws the correct exception;
// - the thrown error contains the escaped offending text;
// - if the error must contain a specific substring, it is present.
// The optional alsoContains is used to verify the last point.
// If it is left at its default value, matching it always returns true, so tests that don't care for it are unaffected.
static void checkRejects (std::string_view testString, std::string_view alsoContains = {""}) {
    // The function hmeigens::detail::escape is tested independently in another file, therefore its behaviour is not verified again here.
    const std::string escaped = hmeigens::detail::escape(testString);
    // CAPTURE prints the captured value at the time of capture if the test doesn't pass.
    // Capturing testString here makes no sense.
    // Printing testString breaks diagnostics messages if they have control characters in them.
    CAPTURE(escaped, alsoContains);
    CHECK_THROWS_MATCHES(
        hmeigens::parseComplex(testString),
        hmeigens::ParseError,
        Catch::Matchers::MessageMatches(
            Catch::Matchers::ContainsSubstring(std::string{alsoContains})
            &&
            Catch::Matchers::ContainsSubstring(escaped)
        )
    );
}

TEST_CASE("Parse test: real number.", "[parse]") {
    // GIVEN a plain real number
    // WHEN  it is parsed
    // THEN  its result is the expected number with no imaginary part
    checkParse ("0", 0.0, 0.0);
    checkParse ("6", 6, 0.0);
    checkParse ("+6", 6, 0.0);
    checkParse ("-6", -6, 0.0);
    checkParse ("6.7", 6.7, 0.0);
    checkParse (".7", .7, 0.0);
    checkParse ("6.", 6., 0.0);
    checkParse ("-6.7", -6.7, 0.0);
    checkParse ("+6.7", 6.7, 0.0);
}

TEST_CASE("Parse test: exponential form.", "[parse]") {
    // GIVEN a real number in exponential form
    // WHEN  it is parsed
    // THEN  its result is the expected number with no imaginary part
    checkParse ("1e0", 1.0, 0.0);
    checkParse ("1e-0", 1.0, 0.0);
    checkParse ("1.e-7", 1e-7, 0.0);
    checkParse ("-2.34e10", -2.34e10, 0.0);
    checkParse ("1E3", 1000.0, 0.0);
    checkParse ("0E3", 0.0, 0.0);
    checkParse (".1e7", 1000000, 0.0);
    checkParse ("1.e2", 100.0, 0.0);
    checkParse ("1e+4", 10000.0, 0.0);
}

TEST_CASE("Parse test: imaginary number.", "[parse]") {
    // GIVEN an imaginary number in algebraic form
    // WHEN  it is parsed
    // THEN  its result is the expected number with no real part
    checkParse ("i", 0.0, 1.0);
    checkParse ("3i", 0.0, 3.0);
    checkParse ("0i", 0.0, 0.0);
    checkParse ("I", 0.0, 1.0);
    checkParse ("-i", 0.0, -1.0);
    checkParse ("+i", 0.0, 1.0);
    checkParse ("1.1i", 0.0, 1.1);
    checkParse ("-3e14i", 0.0, -3e14);
    checkParse (".7i", 0.0, 0.7);
    checkParse ("3.i", 0.0, 3.0);
}

TEST_CASE("Parse test: algebraic form.", "[parse]") {
    // GIVEN a complex number in algebraic form
    // WHEN  it is parsed
    // THEN  its result is the expected number
    checkParse ("2.3-.4i", 2.3, -0.4);
    checkParse ("2+i", 2.0, 1.0);
    checkParse ("2-i", 2.0, -1.0);
    checkParse ("2+6i", 2.0, 6.0);
    checkParse ("2+6I", 2.0, 6.0);
    checkParse ("2.1-6e-3i", 2.1, -6e-3);
}

TEST_CASE("Parse test: ordered pair.", "[parse]") {
    // GIVEN an ordered pair a,b
    // WHEN  it is parsed
    // THEN  its result is the expected number
    checkParse ("2.1,-6e-3", 2.1, -6e-3);
    checkParse ("0,4", 0.0, 4.0);
    checkParse ("4,0", 4.0, 0.0);
    checkParse ("-4,-8e2", -4.0, -800.0);
}

TEST_CASE("ParseError test: the offending text is escaped in the message.", "[parse]") {
    // GIVEN text with control characters
    // WHEN  a hmeigens::ParseError is built with it
    // THEN  the message carries the escaped text
    CHECK_THAT(
        hmeigens::ParseError("4\0.2"sv).what(),
        Catch::Matchers::ContainsSubstring(R"(4\000.2)")
    );
    CHECK_THAT(
        hmeigens::ParseError("What\twent\nwrong?"sv, "The eggs were raw."sv).what(),
        Catch::Matchers::ContainsSubstring(R"(What\twent\nwrong?)")
    );
    // The errorMessage is not escaped, callers escape their text.
    // This is to catch if it is escaped twice.
    CHECK_THAT(
        hmeigens::ParseError("", R"(These\ncharacters\\would\"be\\adoubled.)").what(),
        Catch::Matchers::ContainsSubstring(R"(These\ncharacters\\would\"be\\adoubled.)")
    );
}

TEST_CASE("Parse rejection test: text that is not a complex number.", "[parse]") {
    // GIVEN text that is not a complex number in an accepted form
    // WHEN  it is parsed
    // THEN  a ParseError is thrown with a message carrying the escaped offending text
    checkRejects("");
    checkRejects("  ");
    checkRejects(".");
    checkRejects("..");
    checkRejects(".i");
    checkRejects("ii");
    checkRejects("1.1.i");
    checkRejects("1.1.");
    checkRejects("i.1");
    checkRejects(".e2");
    checkRejects("+");
    checkRejects("1.2.3");
    checkRejects("1.2.3i");
    checkRejects("1,2,3");
    checkRejects("1-2");
    checkRejects("-1+2");
    checkRejects("6,7i");
    checkRejects("1.🤬i");
    checkRejects("悪い入力");
    checkRejects("𓃥𓃠𓆉𓆏𓃯𓃱𓃰");
    checkRejects("𓂀𓂀𓂀𓋹𓁈𓃠𓆃☥𓅓𓆣");
    checkRejects("\0"sv);
    checkRejects("6\0" "7i"sv);
    checkRejects("6\0.7i"sv);
    checkRejects("6.2\0"sv);
    checkRejects("\"3.3\""sv);
    checkRejects(R"(6\0.7i)");
}

TEST_CASE("Parse rejection test: values out of range.", "[parse]") {
    // GIVEN a value that is out of range
    // WHEN  it is parsed
    // THEN  a ParseError is thrown with a message carrying the entire offending text and the "out of range" information
    checkRejects("1e400", "out of range");
    checkRejects("1e-500", "out of range");
    checkRejects("3+1e1000i", "1e1000 is out of range");
    checkRejects("1.8e-9999-i", "1.8e-9999 is out of range");
}

TEST_CASE("Parse rejection test: not yet implemented but planned forms.", "[parse][future]") {
    // GIVEN text that is a complex number in a not yet accepted form
    // WHEN  it is parsed
    // THEN  a ParseError is thrown
    checkRejects(" 1,2");
    checkRejects(" 3,4 ");
    checkRejects("(1,3)");
    checkRejects("2+i3");
    checkRejects("2i-3");
}

TEST_CASE("Parse rejection test: ParseError can be caught as a std::invalid_argument exception.", "[parse]") {
    // GIVEN something that doesn't recognize ParseError
    // WHEN  an invalid input is parsed
    // THEN  the exception can still be caught as std::invalid_argument
    CHECK_THROWS_AS(hmeigens::parseComplex("?!?"), std::invalid_argument);
}
