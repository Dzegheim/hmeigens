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

TEST_CASE("Parse test for exponential form.", "[parse]") {
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

TEST_CASE("Parse test for an imaginary number.", "[parse]") {
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

TEST_CASE("Parse test for algebraic form.", "[parse]") {
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

TEST_CASE("Parse test for ordered pair.", "[parse]") {
    // GIVEN an ordered pair a,b
    // WHEN  it is parsed
    // THEN  its result is the expected number
    checkParse ("2.1,-6e-3", 2.1, -6e-3);
    checkParse ("0,4", 0.0, 4.0);
    checkParse ("4,0", 4.0, 0.0);
    checkParse ("-4,-8e2", -4.0, -800.0);

}

TEST_CASE("Rejection test for text that is not a complex number.", "[parse]") {
    // GIVEN  text that is not a complex number in an accepted form
    // WHEN   it is parsed
    // THEN   a ParseError is thrown
    CHECK_THROWS_AS(hmeigens::parseComplex(""), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("  "), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("."), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex(".."), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex(".i"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("ii"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("1.1.i"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("1.1."), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("i.1"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex(".e2"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("+"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("1,2,3"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("1.2.3"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("1-2"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("-1+2"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("6,7i"), hmeigens::ParseError);
}

TEST_CASE("Rejection test for not yet implemented but planned forms.", "[parse][future]") {
    // GIVEN  text that is a complex number in a not yet accepted form
    // WHEN   it is parsed
    // THEN   a ParseError is thrown
    CHECK_THROWS_AS(hmeigens::parseComplex("(1,3)"), hmeigens::ParseError);
    CHECK_THROWS_AS(hmeigens::parseComplex("2i-3"), hmeigens::ParseError);
}
