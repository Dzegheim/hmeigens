#include <catch2/catch_test_macros.hpp>

TEST_CASE("Catch2 runs test: if this fails there's nowhere left to hide.", "[is_it_alive]") {
    // GIVEN a build with Catch2 and ctest
    // WHEN  the test suite is run
    // THEN  this runs proving that the test framework exists and functions
    
    // The assertion is deliberately trivial and can never fail for any reason connected to this project. If it fails, the testing suite is broken, not the project.
    REQUIRE(2 + 2 == 4);
}
