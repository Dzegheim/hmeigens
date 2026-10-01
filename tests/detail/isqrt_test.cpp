#include "hmeigens/detail/isqrt.hpp"
#include "hmeigens/constants.hpp"

#include <cstddef>  // For std::size_t
#include <limits>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Integer square root test: values.", "[isqrt]") {
    // GIVEN an std::size_t
    // WHEN  its integer square root is computed
    // THEN  the value is the correct one.
    //
    // Trivial values.
    CHECK(hmeigens::detail::isqrt(0) == 0);
    CHECK(hmeigens::detail::isqrt(1) == 1);
    // First set of non-trivial values;
    CHECK(hmeigens::detail::isqrt(2) == 1);
    CHECK(hmeigens::detail::isqrt(3) == 1);
    CHECK(hmeigens::detail::isqrt(4) == 2);
    // Odd exact integer root, and two surrounding values.
    CHECK(hmeigens::detail::isqrt(8) == 2);
    CHECK(hmeigens::detail::isqrt(9) == 3);
    CHECK(hmeigens::detail::isqrt(10) == 3);
    // Even exact integer root, and two surrounding values.
    CHECK(hmeigens::detail::isqrt(15) == 3);
    CHECK(hmeigens::detail::isqrt(16) == 4);
    CHECK(hmeigens::detail::isqrt(17) == 4);
    // Maximum value.
    // Here hmeigens::maxSquarableSize is by definition the maximum squarable std::size_t.
    CHECK(hmeigens::detail::isqrt(std::numeric_limits<std::size_t>::max()) == hmeigens::maxSquarableSize);
}
