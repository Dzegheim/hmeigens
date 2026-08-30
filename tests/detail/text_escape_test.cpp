#include "hmeigens/detail/text_escape.hpp"

#include <string>
#include <string_view>
using namespace std::string_view_literals;      // For ""sv

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Displayable chars test: isDisplayable correctly identifies displayability of special characters.", "[escape]") {
    // GIVEN ASCII and Non-ASCII characters
    // WHEN  they are passed to hmeigens::detail::isDisplayable
    // THEN  the function correctly reports on their displayability in std::locale::classic()
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
