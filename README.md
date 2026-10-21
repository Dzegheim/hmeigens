# HMEigenS
[HMEigenS](https://github.com/Dzegheim/hmeigens) (Householder Matrix Eigenvalues Solver) is a program aimed at finding the eigenvalues of matrices with arbitrary complex elements.

The solver will be based on the Francis QR step.

## Documentation
Documentation for HMEigenS can be found in two places:
- all public interfaces are documented with Doxygen [here](https://dzegheim.github.io/hmeigens);
- all .cpp files, all .hpp in `include/hmeigens/detail/`, all files in `tests/` and all files in `benchmarks/` have extensive documentation, so if someone wishes to review the code it shouldn't be a madness-inducing experience.

## Status
Current version: 0.6.0.

### Implemented
- Parsing of complex numbers [in various forms](https://dzegheim.github.io/hmeigens/namespacehmeigens.html#parser_formats).
- Square matrix class and multiplication.

### In development
- Eigenvalue search.

## Requirements
- A C++ compiler that supports `C++20`, plus `std::format` and `std::from_chars` for floating-point numbers (the code was developed using GCC 15.2.0 via MinGW-w64 11.0.1).
- [CMake](https://cmake.org/) [3.21](https://cmake.org/cmake/help/v3.21/index.html) or newer (tested with 4.4.0).
- [Ninja](https://ninja-build.org/) (tested with [1.13.2](https://github.com/ninja-build/ninja/releases/tag/v1.13.2)). This is necessary to use the existing presets, if the code is built without them, this is not necessary.
- [Catch2](https://github.com/catchorg/Catch2) [3.15.3](https://github.com/catchorg/Catch2/releases/tag/v3.15.3) (note: fetched automatically via CMake, no previous download required).
- [OPTIONAL] [Doxygen](https://www.doxygen.nl) [1.18.0](https://github.com/doxygen/doxygen/releases/tag/Release_1_18_0) for building the documentation locally. Note that formulas are rendered with [MathJax](https://www.mathjax.org/) 3, so they are only visible with an active internet connection. To render them offline a local copy of MathJax is needed to set `MATHJAX_RELPATH` to. 

## Compiling with CMake

### Configuration presets

HMEigenS comes with 10 presets. It is always possible to run
```bash
cmake --list-presets
```
to see all available presets.

### Configuration
HMEigenS can be built in `release` or `debug` mode.

A base configuration with default options can be done by running either one of the following commands
```bash
cmake --preset release
```
```bash
cmake --preset debug
```
by choosing the mode the project is to be built in.

The project can be built by selecting either `double` or `float` as the base floating-point number type. The default build is in `double`, but appending `-f` to any preset switches to the float build, e.g. the two previous commands would be
```bash
cmake --preset release-f
```
```bash
cmake --preset debug-f
```
and would create their own directories in `hmeigens/build/`. There are also other modes that can be selected, each available in both `double` and `float`:
- `release-nt`/`release-nt-f` (`nt` meaning *no tests*) prevents tests from being built;
- `coverage`/`coverage-f` builds in `debug` mode with GCOV coverage (GCC/Clang only);
- `benchmark`/`benchmark-f` builds the benchmarks, but not the tests.

### Building and Testing
Once a configuration is performed successfully, it can be built as
```bash
cmake --build --preset release # Name of the configured preset
```
while tests can be run, if built, using `ctest` as
```bash
ctest --preset release # Name of the built preset.
```
Note that:
- some tests are platform dependent, as they can only be performed on 32-bit architectures (or any other in which `std::size_t` has fewer digits than `std::uintmax_t`);
- some tests are build dependent, as they can only be performed in debug mode;
- some tests are stubs for not yet implemented features, so they are skipped.

### Without presets
Should presets not be used, it is also possible to run CMake without them. These are all the available options:
- `CMAKE_BUILD_TYPE` (no default, since the project is unreleased);
- [`HMEIGENS_SCALAR`](https://dzegheim.github.io/hmeigens/namespacehmeigens.html#scalar_types) (default: `double`) can be either `double` or `float`, and changes the selected floating-point type;
- `HMEIGENS_BUILD_TESTS` (default: `ON`) enables testing;
- `HMEIGENS_BUILD_BENCHMARKS` (default: `OFF`) enables benchmarks (this is only meaningful in release mode);
- `HMEIGENS_COVERAGE` (default: `OFF`, GCC/Clang only) enables coverage (this is only meaningful in debug mode);
- [`HMEIGENS_HYPOT_ULPS`](https://dzegheim.github.io/hmeigens/tolerances_8hpp.html#hypot_ulps) (default: `2`) is a parameter that increases tolerance for tests on the computation of a matrix norm. It is recommended to check the documentation before changing this value.

```bash
# This builds the release project with float as the selected scalar type.
cmake -S . -B build/manual -G Ninja -DCMAKE_BUILD_TYPE=Release -DHMEIGENS_SCALAR=float
cmake --build build/manual
ctest --test-dir build/manual --output-on-failure
```

The previous example assumes a single-configuration generator. For multi-configuration ones (like Visual Studio) use `--config Release|Debug` when building and `-C Release|Debug` when running tests.

### Benchmarks
Benchmarks are performed with Catch2.
The available benchmarks are (by Catch2 tag):
- `complex_mult` to bench the standard `operator*` from `<complex>` versus an explicit multiplication at matrix sizes `10`, `50`, and `200`;
- `matrix_mult` to bench three different approaches to matrix multiplication at matrix sizes `10`, `50`, and `200`;
- `matrix_mult_l` (hidden) same as before, but with size `1000`;
- `matrix_mult_xl` (hidden) same as before, but with size `2000`.

Below is an example of usage for some benchmarks in this project.
```bash
# Configure and build.
# This is the standard double build. To build in float use benchmark-f.
cmake --preset benchmark
cmake --build --preset benchmark
# Go in the benchmark folder.
cd ./build/benchmark/benchmarks/
# See all non-hidden benchmarks.
./hmeigens_benchmarks --list-tests
# See all available benchmarks.
./hmeigens_benchmarks --list-tests "[benchmark]"
# Run all non-hidden benchmarks.
# Note: by default Catch2 has --benchmark-samples 100.
./hmeigens_benchmarks
# Run a specific one, e.g. the matrix multiplication one.
# It is also possible to change the number of samples, e.g. 200.
./hmeigens_benchmarks "[matrix_mult]" --benchmark-samples 200
# Run a hidden case.
# It is not recommended to use a large number of samples here.
# These executions are long and hidden for a reason.
./hmeigens_benchmarks "[matrix_mult_xl]" --benchmark-samples 20
```

### Coverage
Coverage is performed with gcov. It is included in GCC by default, and is available in Clang with `llvm-cov gcov`.
Below is an example of usage.
```bash
# Configure and build.
# This is the standard double build. To build in float use coverage-f.
cmake --preset coverage
cmake --build --preset coverage
# Run the tests.
ctest --preset coverage
```
Coverage files can be found in `./build/coverage/CMakeFiles/hmeigens_core.dir/` and from there by following the structure of the repository, so by default in `src/hmeigens` and `src/hmeigens/detail`.

Note that coverage is additive, so if the `.gcda` files are already present and the tests are run again, the amount of times each line was run will not reset but increase from where it was already. Simply delete them to reset the count.

## Platforms
[![CI-Windows](https://github.com/Dzegheim/hmeigens/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/Dzegheim/hmeigens/actions/workflows/ci-windows.yml)
[![CI-Linux](https://github.com/Dzegheim/hmeigens/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/Dzegheim/hmeigens/actions/workflows/ci-linux.yml)
[![CI-macOS](https://github.com/Dzegheim/hmeigens/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/Dzegheim/hmeigens/actions/workflows/ci-macos.yml)

In addition to the setup already listed in *Requirements*, the codebase is built, run, and tested on all the systems listed in the [workflow files](.github/workflows/).

The four main presets (`release`, `debug`, `release-f`, `debug-f`) are run on each major family of OSs. The table below lists the tested combinations. For more details check the files. OSs with a * below only run `release` and `debug-f`.

| **Operating System** | **Architecture(s)** | **Compiler(s)** | **Notes** |
|---|---|---|---|
| **Ubuntu** | | | |
| Ubuntu 24.04 | x86-64 | GCC 13, 14; Clang 17, 18 | |
| Ubuntu 24.04 | x86 (`-m32`) | GCC 14 | Build is 32-bit. |
| Ubuntu 26.04 | x86-64 | GCC 15; Clang 22 | |
| Ubuntu 26.04 | ARM64 | GCC 15 | |
| **Other Linux** | | | |
| Debian 13* | x86-64 | GCC 14 | Container, not native. |
| Fedora 44* | x86-64 | GCC 16 | Container, not native. |
| Alpine 3.24* | x86-64 | GCC 15 | Container, not native. Uses `musl` C library. |
| **Windows Server** | | | |
| Windows Server 2022 | x64, x86 | MSVC (VS 2022) | The x86 build is 32-bit. |
| Windows Server 2025 | x64, x86 | MSVC (VS 2026) | The x86 build is 32-bit. |
| Windows Server 2025 | x64 | clang-cl (default) | Default preinstalled clang-cl version. |
| Windows Server 2025, MSYS2 UCRT64 | x64 | GCC `latest` | Due to MSYS2, GCC is `latest`. |
| Windows Server 2025, MSYS2 CLANG64 | x64 | Clang `latest` | Uses `libc++`. Due to MSYS2, Clang is `latest`. |
| Windows Server 2025, MSYS2 MINGW32 | x86 | GCC `latest` | Due to MSYS2, GCC is `latest`. Build is 32-bit. |
| **Windows 11** | | | |
| Windows 11 | ARM64 | MSVC (VS 2026) |  |
| **macOS** | | | |
| macOS 15 | ARM64 | GCC 15 | |
| macOS 26 | ARM64 | Apple Clang (default) | Default preinstalled Apple Clang version. |
| macOS 26 | x86-64 | Apple Clang (default) | Default preinstalled Apple Clang version. |

Note that the major version is fixed where possible or convenient. Minor versions may vary between runs.

## A Personal Note
This code is developed as a project for a university exam. It is also an excuse for picking back up C++ and catch up on some more modern programming concepts (such as `<concepts>`, if you excuse the joke).
The original version of this code was developed in C++14 for an introductory C++ course in 2017/2018. This is a modernised and improved version that introduces:
- code split in various files, rather than a single monolith;
- testing (the original had none);
- version control (the original had `(n)` stapled to the file name once an update was *big enough*);
- fixes for several uncaught bugs and unoptimized portions.

While there may be some parts of this project that look over-engineered (see the aforementioned `<concepts>`), this was a deliberate choice made to try and use more advanced parts of C++20.

## Changelog
See [the changelog](CHANGELOG.md).

## License and Attribution
This code is released under the [GNU Affero General Public License v3.0](LICENSE).

## Italian version
Coming soon.
