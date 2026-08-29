include(FetchContent)

# Testing is performed with Catch2. Options to enable/disable them are controlled by the HMEIGENS_BUILD_TESTS variable.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.15.3
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME_LOWER}_tests is_it_alive_test.cpp complex_parse_test.cpp square_matrix_test.cpp)
target_link_libraries(${PROJECT_NAME_LOWER}_tests PRIVATE ${PROJECT_NAME_LOWER}_core ${PROJECT_NAME_LOWER}_flags Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME_LOWER}_tests)
