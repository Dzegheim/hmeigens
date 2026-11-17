/// @file
/// @brief The main eigenvalue solver.
// Declarations (hmeigens):
//     - makeUpperHessenbergInPlace(SquareMatrix&);
//     - makeUpperHessenberg(SquareMatrix).
#ifndef HMEIGENS_EIGENVALUES_SOLVER_HPP
#define HMEIGENS_EIGENVALUES_SOLVER_HPP

#include "hmeigens/square_matrix.hpp"

namespace hmeigens {

    /// @brief Transforms a matrix into a similar matrix in upper Hessenberg form via Householder reflections.
    ///
    /// Given a matrix \f(\mathbf{A}\f) of size \f(n\f), it is always possible to transform it into a similar matrix \f(\mathbf{A}_H\f) in upper Hessenberg form. This transformation is done via a unitary matrix \f(\mathbf{U}\f) that is the product of \f(n-2\f) Householder matrices
    /// \f[
    /// \mathbf{U} = \prod_{i=1}^{n-2} \mathbf{H}_i\ ,
    /// \f]
    /// so that
    /// \f[
    /// \mathbf{A}_H = \mathbf{U}^\dagger \mathbf{A} \mathbf{U}\ .
    /// \f]
    /// Let \f(\mathbf{v}_i\f) be a column vector (i.e. an \f((n-i)\times 1\f) matrix) holding all the elements below the diagonal for the \f(i\f)-th column of \f(\mathbf{A}\f). Let \f(\mathbf{u}_i=\mathbf{v}_i-\phi\left\|\mathbf{v}_i\right\|\mathbf{e}_{1i}\f), with \f(\mathbf{e}_{1i}\f) the first canonical basis unit vector of \f(\mathbb{C}^{n-i}\f), and \f(\left|\phi\right|=1\f). The number \f(\phi\f) is chosen to have the phase opposite to that of the first element of \f(\mathbf{v}_i\f). The Householder reflector for \f(\mathbf{v}_i\f) will be
    /// \f[
    /// \mathbf{H}_i = \mathbf{I} - 2\frac{\mathbf{u}_i\mathbf{u}^{\dagger}_i}{\left\|\mathbf{u}_i\right\|^2}\ ,
    /// \f]
    /// and applying it to \f(\mathbf{A}\f) as \f(\mathbf{H}_i\mathbf{A}\mathbf{H}_i\f) makes it so that the elements below the diagonal for the \f(i\f)-th column of \f(\mathbf{A}\f) become \f((\phi\left\|\mathbf{v}_i\right\|, 0, \dots, 0)\f). Repeating this process for all the first \f(n-2\f) columns of \f(\mathbf{A}\f) transforms it into \f(\mathbf{A}_H\f).
    ///
    /// See *Matrix Computations* (Golub & Van Loan, 4th edition, 2013) for a much more elaborate discussion of this process, in particular section 7.4.3 **The Hessenberg Reduction**.
    /// @param matrix The matrix to be transformed into upper Hessenberg form.
    /// @return A reference to the given `hmeigens::SquareMatrix`, modified to be in upper Hessenberg form.
    /// @throws std::bad_alloc if memory cannot be allocated to construct an `hmeigens::SquareMatrix::Container` that is at least `matrix.size()-1` long.
    /// @warning This function's correctness is untested as of now.
    /// @sa hmeigens::makeUpperHessenberg
    SquareMatrix& makeUpperHessenbergInPlace(SquareMatrix& matrix);

    /// @brief Computes an upper Hessenberg matrix similar to the input, via Householder reflections.
    ///
    /// See `hmeigens::makeUpperHessenbergInPlace` for information on the algebra. The only difference is that this function returns a copy and does not alter the input.
    /// @param matrix The matrix whose upper Hessenberg matrix form is computed.
    /// @return A matrix in upper Hessenberg form that is similar to the input.
    /// @throws std::bad_alloc if memory cannot be allocated to construct an `hmeigens::SquareMatrix::Container` that is at least `matrix.size()-1` long.
    /// @throws std::bad_alloc if memory to construct `matrix` cannot be allocated on the machine.
    /// @warning This function's correctness is untested as of now.
    /// @sa hmeigens::makeUpperHessenbergInPlace
    [[nodiscard]] SquareMatrix makeUpperHessenberg(SquareMatrix matrix);
}

#endif
