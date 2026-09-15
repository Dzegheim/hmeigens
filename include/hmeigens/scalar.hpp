/// @file
/// @brief A compile-time switch between `double` (default), `float`.
///
/// Selected via CMake `-DHMEIGENS_SCALAR=float|double`.
///
/// @note As of v0.3.0 `long double` is unsupported. See [CHANGELOG.md](CHANGELOG.md) for details.
#ifndef HMEIGENS_SCALAR_HPP
#define HMEIGENS_SCALAR_HPP
#include <complex>
#include <string>

namespace hmeigens {

    #if defined(HMEIGENS_SCALAR_FLOAT)
    using Scalar = float;
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
