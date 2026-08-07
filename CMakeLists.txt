
cmake_minimum_required(VERSION 3.21)

project(HMEigenS
    VERSION 0.2.0
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

# Testing is performed with Catch2 (https://github.com/catchorg/Catch2).
# Below are the options to build the tests.
option(HMEIGENS_BUILD_TESTS "Build the test suite." ON)

if(HMEIGENS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")