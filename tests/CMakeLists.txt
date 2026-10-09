include(FetchContent)

# Testing is performed with Catch2. Options to enable/disable them are controlled by the HMEIGENS_BUILD_TESTS variable.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.15.3
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(Catch2)

# Source code for the tests.
set(HMEIGENS_TEST_SOURCES
    is_it_alive_test.cpp
    complex_parse_test.cpp
    detail/text_escape_test.cpp
    detail/to_scalar_test.cpp
    square_matrix_test.cpp
    detail/square_matrix_helpers_test.cpp
    scalar_test.cpp
    detail/isqrt_test.cpp
    )

add_executable(${PROJECT_NAME_LOWER}_tests ${HMEIGENS_TEST_SOURCES})
target_link_libraries(${PROJECT_NAME_LOWER}_tests PRIVATE ${PROJECT_NAME_LOWER}_core ${PROJECT_NAME_LOWER}_flags Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME_LOWER}_tests)

# This is a multiplicative constant for the number of ULPs a computed norm has as tolerance for the tests of the function hmeigens::SquareMatrix::norm().
# The actual tolerance is HMEIGENS_HYPOT_ULPS*size^2, where size is the size of the matrix.
# The default is set to 2. Reasoning is provided in the documentation for hmeigens::normTolerance.
# This value is left modifiable if the tests fail due to implementation-specific rounding errors (which matter less and less for large matrices, as the odds of all size^2 errors moving in the same direction are low).
set(HMEIGENS_HYPOT_ULPS 2 CACHE STRING "This is a multiplicative constant for the number of ULPs a computed norm has as tolerance for the tests of the function hmeigens::SquareMatrix::norm(). See documentation for details.")
# Private to prevent carrying over flags.
target_compile_definitions(${PROJECT_NAME_LOWER}_tests PRIVATE HMEIGENS_HYPOT_ULPS=${HMEIGENS_HYPOT_ULPS})
