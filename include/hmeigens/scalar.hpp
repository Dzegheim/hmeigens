/// @file
/// @brief A compile-time switch between `double` (default), `float`.
///
/// Selected via CMake `-DHMEIGENS_SCALAR=float|double`.
///
/// @note As of v0.3.0 `long double` is unsupported. See [CHANGELOG.md](CHANGELOG.md) for details.
#ifndef HMEIGENS_SCALAR_HPP
#define HMEIGENS_SCALAR_HPP

/// @brief Preprocessor macro used to prevent implicit conversions.
///
/// The macro simply appends the suffix `f` in the `float` build, and leaves the value unchanged in the `double` build.
/// Since this code can be compiled in both modes via the `-DHMEIGENS_SCALAR_FLOAT` flag, typing something like `hmeigens::Scalar value = 1.23456;` works perfectly when the selected scalar type is `double`, but can alter the value when it is `float`.
/// This is because [the value is rounded twice](https://www.exploringbinary.com/double-rounding-errors-in-floating-point-conversions/), both by the compiler when changing the literal to a `double`, and then when it is converted to `float`.
/// To avoid this issue, values in the code are wrapped by this macro.
/// @note If a value has neither `.` nor an exponent, e.g. `HMEIGENS_MAKE_SCALAR(6)`, it could either be a compile error (for `float`) or an `int` (for `double`). Values in the code always have a trailing `.`, even when they're whole numbers.
/// @param number The value of the number that may need the suffix appended.
#ifdef HMEIGENS_SCALAR_FLOAT
#define HMEIGENS_MAKE_SCALAR(number) number##f
#else
#define HMEIGENS_MAKE_SCALAR(number) number
#endif

#include <complex>
#include <string>

namespace hmeigens {

    #if defined(HMEIGENS_SCALAR_FLOAT)
    /// @brief Alias for the floating-point type used throughout the project.
    ///
    /// Due to algorithm precision reasons the default is set to `double` rather than `float`.
    /// The reference notes for this project (G. Servizi, *Autovalori*, 2017) show residual errors of the order `1e-8`.
    /// Since `float` has only 7 decimal digits, it may not pass the more stringent tests.
    using Scalar = float;

    /// @brief An `std::string` variable naming the selected type.
    ///
    /// Useful for letting users know at runtime the floating-point type used to build the project.
    inline const std::string scalarType = "float";
    #else

    /// @brief Alias for the floating-point type used throughout the project.
    ///
    /// Due to algorithm precision reasons the default is set to `double` rather than `float`.
    /// The reference notes for this project (G. Servizi, *Autovalori*, 2017) show residual errors of the order `1e-8`.
    /// Since `float` has only 7 decimal digits, it may not pass the more stringent tests.
    using Scalar = double;

    /// @brief An `std::string` variable naming the selected type.
    ///
    /// Useful for letting users know at runtime the floating-point type used to build the project.
    inline const std::string scalarType = "double";
    #endif

    /// @brief Alias for complex numbers with the chosen `hmeigens::Scalar` value type.
    using Complex = std::complex<Scalar>;
}

#endif
