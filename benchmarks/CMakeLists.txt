# Benchmarking is performed with Catch2. Options to enable/disable it are controlled by the HMEIGENS_BUILD_BENCHMARKS variable.
# Its dependency is declared in the top-level CMakeLists.txt.
FetchContent_MakeAvailable(Catch2)

# Source code for the benchmarks.
set(HMEIGENS_BENCHMARK_SOURCES
    complex_multiplication_bench.cpp
    )

add_executable(${PROJECT_NAME_LOWER}_benchmarks ${HMEIGENS_BENCHMARK_SOURCES})
target_link_libraries(${PROJECT_NAME_LOWER}_benchmarks PRIVATE ${PROJECT_NAME_LOWER}_core ${PROJECT_NAME_LOWER}_flags Catch2::Catch2WithMain)
