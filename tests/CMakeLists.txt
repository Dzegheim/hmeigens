# Testing is performed with Catch2. Options to enable/disable them are controlled by the HMEIGENS_BUILD_TESTS variable.
# Its dependency is declared in the top-level CMakeLists.txt.
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

# Below are variables that may be useful for testing.

# This is a multiplicative constant for the number of ULPs a computed norm has as tolerance for the tests of the function hmeigens::SquareMatrix::norm().
# The actual tolerance is HMEIGENS_HYPOT_ULPS*2*size^2, where size is the size of the matrix.
# The default is set to 2 in the library. Reasoning is provided in the documentation for HMEIGENS_HYPOT_ULPS in tolerances.hpp.
# This value is left modifiable if the tests fail due to implementation-specific rounding errors (which matter less and less for large matrices, as the odds of all size^2 errors moving in the same direction are low).
set(HMEIGENS_HYPOT_ULPS "" CACHE STRING "This is a multiplicative constant for the number of ULPs a computed norm has as tolerance for the tests of the function hmeigens::SquareMatrix::norm(). Uses the default in tolerances.hpp if empty. See documentation for details.")
if(NOT HMEIGENS_HYPOT_ULPS STREQUAL "")
    if(NOT HMEIGENS_HYPOT_ULPS MATCHES "^[1-9][0-9]*$")
        message(FATAL_ERROR "HMEIGENS_HYPOT_ULPS must be a positive integer, got \"${HMEIGENS_HYPOT_ULPS}\".")
    endif()
    # Private to prevent carrying over flags.
    target_compile_definitions(${PROJECT_NAME_LOWER}_tests PRIVATE HMEIGENS_HYPOT_ULPS=${HMEIGENS_HYPOT_ULPS})
endif()

# Add coverage instrumentation to tests.
if(HMEIGENS_COVERAGE)
    target_compile_options(${PROJECT_NAME_LOWER}_tests PRIVATE --coverage -O0)
endif()
