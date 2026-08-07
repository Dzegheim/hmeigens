#include <catch2/catch_test_macros.hpp>

TEST_CASE("Catch2 runs. If this fails there's nowhere left to hide.", "[is_it_alive]") {
    REQUIRE(2 + 2 == 4);
}