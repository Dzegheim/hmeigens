#include "hmeigens/detail/to_scalar.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/complex_parse.hpp"

#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

// This is a helper function to verify that the number read corresponds to the expected one.
// The expectedValue is a double because catch2 always checks in double anyway.
// See comment to checkParse in tests/complex_parse_test.cpp.
static void checkReads (std::string_view toConvert, double expectedValue) {
    CAPTURE(toConvert, expectedValue);
    constexpr double dblParseTolerance = static_cast<double>(hmeigens::parseTolerance);
    CHECK_THAT(
        // The second parameter of hmeigens::detail::toScalar is only ever used for error message construction.
        // Here it is left empty as this helper only ever checks inputs that should convert.
        static_cast<double> (
            hmeigens::detail::toScalar(toConvert, "")
        ),
        Catch::Matchers::WithinAbs(expectedValue, dblParseTolerance)
        ||
        Catch::Matchers::WithinRel(expectedValue, dblParseTolerance)
    );
}

// This is a helper function to test whether text that is not supposed to be parsed:
// - throws;
// - throws the correct exception;
// - the exception contains the expected text;
// - the exception contains the context text.
// The contextText is defaulted to empty, as not all test cases need it.
static void checkRejects (std::string_view testString, std::string_view expectedText, std::string_view contextText = {""}) {
    CAPTURE(testString, expectedText, contextText);
    CHECK_THROWS_MATCHES(
        hmeigens::detail::toScalar(testString, contextText),
        hmeigens::ParseError,
        Catch::Matchers::MessageMatches(
            Catch::Matchers::ContainsSubstring(std::string{expectedText})
            &&
            Catch::Matchers::ContainsSubstring(std::string{contextText})
        )
    );
}

TEST_CASE("Text to scalar test: real numbers.", "[to_scalar]") {
    // GIVEN text in the form of an appropriate real number
    // WHEN  it is parsed
    // THEN  the result is the expected number
    checkReads ("6", 6.);
    checkReads ("-4.2", -4.2);
    checkReads (".1", .1);
    checkReads ("2.", 2.);
    checkReads ("1e9", 1e9);
}

TEST_CASE("Text to scalar test: leading \"+\".", "[to_scalar]") {
    // GIVEN text in the form of an appropriate real number with a leading "+"
    // WHEN  it is parsed
    // THEN  the result is the expected number
    checkReads ("+6.7", 6.7);
    checkReads ("+1e-5", 1e-5);
}

TEST_CASE("Text to scalar rejection test: out of range.", "[to_scalar]") {
    // GIVEN text containing a number that is out of range
    // WHEN  it is parsed
    // THEN  the correct exception is thrown with the correct message
    checkRejects("1e400", "out of range");
    checkRejects("1e-500", "out of range");
}

TEST_CASE("Text to scalar rejection test: bad input.", "[to_scalar]") {
    // GIVEN ill formatted text
    // WHEN  it is parsed
    // THEN  the correct exception is thrown with the correct message
    checkRejects("", "This is a bug");
    checkRejects("unexpected input", "This is a bug");
    checkRejects("1.2withstuffafterit", "This is a bug");
    checkRejects("-1a3", "This is a bug");
}

TEST_CASE("Text to scalar rejection test: full context is passed.", "[to_scalar]") {
    // GIVEN ill formatted or out of range text
    // WHEN  it is parsed along with its full context
    // THEN  the correct exception is thrown with the correct message and context
    checkRejects("1e40000", "out of range", "2+1e40000i");
}

TEST_CASE("Text to scalar rejection test: a leading \"+\" is correctly reported.", "[to_scalar]") {
    // GIVEN out of range or ill formatted text with a leading "+"
    // WHEN  it is parsed
    // THEN  in the exception text the leading "+" is conserved
    // This is necessary because std::from_chars does not want a leading "+" as input, but the parsing in complex_parse.cpp (that uses hmeigens::detail::toScalar) accepts it.
    // The leading "+" is manually checked for and discarded within hmeigens::detail::toScalar, but in the event of an error, the original is used to generate the exception.
    checkRejects("+1e3000", "+1e3000");
    checkRejects("+garbage", "+garbage");
}
