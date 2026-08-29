# Changelog

All notable changes to this project are documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/).

Versions before 0.3.0 were not tagged.

Version 0.2.0 is recorded from git history.

## [Unreleased]
### Added
- **Tests for SquareMatrix.** Specifically:
     - A `SquareMatrix` built with a valid size reports it correctly with `size()`.

### Changed
- **SquareMatrix construction now reports out-of-memory errors as `std::bad_alloc`.** An `std::bad_alloc` exception was previously caught and rethrown as an `std::runtime_error` with the original nested in it. Callers catching `std::runtime_error` must now catch `std::bad_alloc` instead. The constructor does not handle any exception now.
- **Manual string concatenation replaced with `std::format` in the exception constructors all around the code.** 
- **Improved error message for `std::length_error` in `square_matrix.cpp`.**

## [0.5.0] - 2026-08-27
### Added
- **SquareMatrix class.** Features:
    - constructor with safety check to ensure a matrix has a reasonable size (no `0`, no absurdly large numbers).
    - getter for the size.
- **maxMatrixSize constant.** Used to check that a requested matrix does not break `std::size_t` boundaries.

## [0.4.0] - 2026-08-26
### Added
- **Comprehensive testing for the parser.**
- **Coverage build.** New flag `-DHMEIGENS_COVERAGE=ON` to compile with [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html). GCC and Clang only.
- **String holding the selected type.** In `scalar.hpp` the new variable `hmeigens::scalarType` now allows for runtime naming of the selected Scalar type.

### Fixed
- **Parser: numbers with a trailing decimal point are now accepted.** The form `6.` is valid in C++ and was rejected by the parser. It is now accepted.
- **Parser: out of range values are now correctly reported as such.** A number too large or too small for the chosen `Scalar` is now properly handled, a message names the part that caused the issue and the type.
    - Added a second constructor to `hmeigens::ParseError` with an error message as a parameter.

## [0.3.0] - 2026-08-22
### Removed
- **Support for `long double`.** The flag `-DHMEIGENS_SCALAR` now accepts `float` or `double` only. Reasoning: its size and precision are implementation defined, so it was more trouble than it was worth. Specifically:
    - [`std::from_chars`](https://en.cppreference.com/cpp/utility/from_chars) does not read them with the required accuracy of 1 ULP:
    >  In any case, the resulting value is one of at most two floating-point values closest to the value of the string matching the pattern, after rounding according to `std::round_to_nearest`. 
    [libstdc++](https://github.com/gcc-mirror/gcc/blob/releases/gcc-15.2.0/libstdc%2B%2B-v3/src/c%2B%2B17/floating_from_chars.cc) verbatim says:
    >  // Either long double is the same as double, or we can't use strtold.
    >  // In the latter case, this might give an incorrect result (e.g. values
    >  // out of range of double give an error, even if they fit in long double).
    - the type is not supported by Catch2's floating-point matchers `WithinRel`, `WithinAbs`.

## [0.2.0] - 2026-08-02
### Added
- CMake build with Ninja, presets, and warnings enabled.
- [Catch2](https://github.com/catchorg/Catch2.git) (test framework).
- Compile-time selection of the floating-point type via `-DHMEIGENS_SCALAR` (float, double, long double).
- Parser for complex numbers written as text.
