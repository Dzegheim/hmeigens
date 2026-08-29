cmake_minimum_required(VERSION 3.21)

project(HMEigenS
    VERSION 0.5.0
    DESCRIPTION "HMEigenS (Householder Matrix Eigenvalues Solver) is a program that searches the eigenvalues of arbitrary square matrices."
    LANGUAGES CXX)

# Lowercase name for identifiers in project, to avoid stylization issues in include paths.
string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)

# Global variables related to standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static libraries by default.
set(BUILD_SHARED_LIBS OFF)

# Tests to check whether some tools are available that may not be.
include(CheckSourceCompiles)
# Verify std::from_chars can read from double.
check_source_compiles(CXX "
    #include <charconv>
    int main() {
        const char* s = \"1.5\";
        double d{};
        std::from_chars(s, s+3, d);
    }
" HMEIGENS_HAS_CHARCONV)
# Verify std::format is available.
check_source_compiles(CXX "
    #include <format>
    int main() {
        std::format(\"{}\", 1);
    }
" HMEIGENS_HAS_FORMAT)
if(NOT (HMEIGENS_HAS_CHARCONV AND HMEIGENS_HAS_FORMAT))
    set(MISSING "")
    if(NOT HMEIGENS_HAS_CHARCONV)
        list(APPEND MISSING "  - std::from_chars for floating-point types")
    endif()
    if(NOT HMEIGENS_HAS_FORMAT)
        list(APPEND MISSING "  - std::format")
    endif()
    list(JOIN MISSING "\n" MISSING_INFO)
    message(FATAL_ERROR
        "HMEigenS requires std::format and std::from_chars for floating-point numbers.\n"
        "Detected compiler (${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}).\n"
        "Missing:\n"
        "${MISSING_INFO}")
endif()


# Interface for the compile flags for the library.
add_library(${PROJECT_NAME_LOWER}_flags INTERFACE)

# Flags for warnings.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME_LOWER}_flags INTERFACE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif(MSVC)
    target_compile_options(${PROJECT_NAME_LOWER}_flags INTERFACE /W4 /permissive-)
endif()

# Link compiler runtime statically.
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(${PROJECT_NAME_LOWER}_flags INTERFACE
        -static -static-libgcc -static-libstdc++)
endif()

# Source code for the project.
set(HMEIGENS_SOURCES
    complex_parse.cpp
    square_matrix.cpp
    detail/text_escape.cpp)
list(TRANSFORM HMEIGENS_SOURCES PREPEND src/hmeigens/)

add_library(${PROJECT_NAME_LOWER}_core STATIC ${HMEIGENS_SOURCES})
target_include_directories(${PROJECT_NAME_LOWER}_core PUBLIC include)
# Private to prevent carrying over flags.
target_link_libraries(${PROJECT_NAME_LOWER}_core PRIVATE ${PROJECT_NAME_LOWER}_flags)

# Selector for the floating-point type in include/hmeigens/scalar.hpp.
# The header documents why double is the default.
set(HMEIGENS_SCALAR "double" CACHE STRING "Floating-point type for all computation: float, double.")
set_property(CACHE HMEIGENS_SCALAR PROPERTY STRINGS float double)
# Public, cannot be private, as scalar.hpp needs these to choose a floating-point type.
# Anything that includes it must be built with the same choice as the library, to prevent mismatches.
if(HMEIGENS_SCALAR STREQUAL "float")
    target_compile_definitions(${PROJECT_NAME_LOWER}_core PUBLIC HMEIGENS_SCALAR_FLOAT)
elseif(NOT HMEIGENS_SCALAR STREQUAL "double")
    message(FATAL_ERROR "HMEIGENS_SCALAR must be float or double (got '${HMEIGENS_SCALAR}').")
endif()
message(STATUS "Floating-point type selected: ${HMEIGENS_SCALAR}.")

# Coverage options.
# Used for testing to verify what parts of the code ACTUALLY run.
# Note: It's only for GCC and Clang.
option(HMEIGENS_COVERAGE "Build with coverage for testing." OFF)
if(HMEIGENS_COVERAGE)
    # Check the appropriate compiler is used.
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "HMEIGENS_COVERAGE requires GCC or Clang.")
    endif()
    # Private for compile but public for linking.
    # Coverage only makes sense for -O0 optimization.
    target_compile_options(${PROJECT_NAME_LOWER}_core PRIVATE --coverage -O0)
    target_link_options(${PROJECT_NAME_LOWER}_core PUBLIC --coverage)
endif()

# Testing is performed with Catch2 (https://github.com/catchorg/Catch2).
# Below are the options to build the tests.
option(HMEIGENS_BUILD_TESTS "Build the test suite." ON)

if(HMEIGENS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
