#include "hmeigens/constants.hpp"
#include "hmeigens/complex_parse.hpp"

#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// Apparently Catch2's floating-point matchers always compare in double (see catch_matchers_floating_point.hpp).
// Source: https://github.com/catchorg/Catch2/blob/v3.15.3/docs/comparing-floating-point-numbers.md#withinabs
static void checkParse (std::string_view testString, double expectedReal, double expectedImag) {
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

TEST_CASE("Parse test for a real number.", "[parse]") {
    // GIVEN the text "6.7"
    // WHEN  it is parsed
    // THEN  the result is 6.7 with no imaginary part
    checkParse ("6.7", 6.7, 0.0);
}
