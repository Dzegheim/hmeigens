#include "hmeigens/constants.hpp"
#include "hmeigens/complex_parse.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("Parse test for a real number.", "[parse]") {
    // GIVEN the text "6.7"
    // WHEN  it is parsed
    // THEN  the result is 6.7 with no imaginary part
    const auto z = hmeigens::parseComplex("6.7");
    REQUIRE_THAT(z.real(), Catch::Matchers::WithinRel(6.7, hmeigens::parseTolerance));
    REQUIRE_THAT(z.imag(), Catch::Matchers::WithinAbs(0.0, hmeigens::parseTolerance));
}