#ifndef HMEIGENS_SCALAR_HPP
#define HMEIGENS_SCALAR_HPP
#include <complex>

namespace hmeigens {
    // A compile-time switch between float, double.
    // Selected via CMake -DHMEIGENS_SCALAR=float|double.
    // Due to algorithm precision reasons the default is set to double rather than float.
    // The reference notes for this project (G. Servizi, Autovalori, 2017) show residual errors of the order 1e-8.
    // Since float has only 7 decimal digits, it may not pass the more stringent tests.
    // NOTE: long double is unsupported (see CHANGELOG.md v0.3.0).
    #if defined(HMEIGENS_SCALAR_FLOAT)
    using Scalar = float;
    #else
    using Scalar = double;
    #endif

    using Complex = std::complex<Scalar>;
}

#endif
