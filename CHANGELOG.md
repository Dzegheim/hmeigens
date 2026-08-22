# Changelog

All notable changes to this project are documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/).

Versions before 0.3.0 were not tagged.

Version 0.2.0 is recorded from git history.

## [Unreleased]
### Added
- **Comprehensive test for the parser.**

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
