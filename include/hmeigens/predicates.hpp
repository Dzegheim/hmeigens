/// @file
/// @brief A collection of functions that verify the properties of a given matrix.
// Declarations (hmeigens):
//     - isFinite(const SquareMatrix&);
//     - getTolerance(const SquareMatrix&);
//     - isUpperHessenberg(const SquareMatrix&, const Scalar).
#ifndef HMEIGENS_PREDICATES_HPP
#define HMEIGENS_PREDICATES_HPP

#include "hmeigens/square_matrix.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/constants.hpp"

namespace hmeigens {

    /// @brief Checks whether all elements of the matrix satisfy `std::isfinite()` for both their real and imaginary parts.
    /// @param matrix The matrix to be checked.
    /// @return `true` if all elements of the matrix are finite, `false` otherwise.
    [[nodiscard]] bool isFinite(const SquareMatrix& matrix);

    /// @brief Computes the tolerance that is used by predicates to assess if numbers verify properties.
    ///
    /// The Francis QR algorithm performs several operations, each introducing roundings. The tolerance adopted here comes from the bounds provided in *Matrix Computations* (Golub & Van Loan, 4th edition, 2013).
    /// In particular the statements that matter here are (keeping the book's notation):
    /// - Section 7.4.2 **Hessenberg Reduction**: after reducing a matrix \f(A\f) to Hessenberg form, this form is similar to (and thus has the same eigenvalues as) \f(A+E\f), where \f(E\f) is a perturbation of \f(A\f) whose norm is \f(\left\|E\right\|_F\leq c\cdot n^2\cdot \mathbf{u}\cdot\left\|A\right\|_F\f), with \f(c\f) a small constant, \f(n\f) the size of the matrix \f(A\f), \f(\mathbf{u}\f) the unit roundoff (i.e. half of `hmeigens::epsilon`), and \f(\left\|\cdot\right\|_F\f) the Frobenius norm (`hmeigens::SquareMatrix::norm`, also \f(\left\|\cdot\right\|\f));
    /// - Section 7.5.6 **QR algorithm**: for the whole process the norm of the perturbation is of the same order of magnitude as \f(\mathbf{u}\cdot\left\|A\right\|_2\f), where \f(\left\|\cdot\right\|_2\f) is the `2`-norm, which satisfies \f(\left\|A\right\|_2 \leq \left\|A\right\|_F \leq \sqrt{n}\left\|A\right\|_2\f) (*ibid.*, 2.3.2).
    ///
    /// In this project the Frobenius norm is used instead of the `2`-norm due to it being easier to compute, and the two are equivalent in practice. The tolerance for this project is therefore chosen as:
    /// \f[
    /// k\cdot n^2\cdot\left\|M\right\|\cdot\varepsilon\ ,
    /// \f]
    /// where \f(M\f) is the matrix and \f(\varepsilon\f) is `hmeigens::epsilon`, while \f(k\f) is `hmeigens::toleranceK`.
    /// The value \f(n^2\f) is included in the tolerance for the whole process since it is part of the Hessenberg reduction bound, and no information is provided about its influence on the whole QR algorithm.
    /// The value of the constant incorporates the factor of `2` between \f(\mathbf{u}\f) and \f(\varepsilon\f), as well as \f(c\f).
    ///
    /// This tolerance is used to determine, for example, when a number is `0` with respect to the matrix to verify triangularity, or any other property described by the other functions of `predicates.hpp`.
    /// @param matrix The matrix to compute the tolerance of.
    /// @return The tolerance computed as \f(k\cdot n^2\cdot\left\|M\right\|\cdot\varepsilon\f).
    /// @sa hmeigens::toleranceK
    /// @sa HMEIGENS_TOLERANCE_K
    [[nodiscard]] Scalar getTolerance(const SquareMatrix& matrix);

    /// @brief Checks whether a matrix is in upper Hessenberg form within a given tolerance.
    ///
    /// A matrix is upper Hessenberg when it is in the form
    /// \f[
    /// M =
    /// \begin{pmatrix}
    ///    m_{1,1} & m_{1,2} & m_{1,3} & \cdots    & m_{1,n}
    /// \\ m_{2,1} & m_{2,2} & m_{2,3} & \cdots    & m_{2,n}
    /// \\ 0       & m_{3,2} & m_{3,3} & \cdots    & m_{3,n}
    /// \\ \vdots  & \ddots  & \ddots  & \ddots    & \vdots
    /// \\ 0       & \cdots  & 0       & m_{n,n-1} & m_{n,n}
    /// \end{pmatrix}\ ,
    /// \f]
    /// i.e. it is *almost* upper triangular.
    /// This function compares all sub-subdiagonal elements' moduli with the given tolerance, to determine whether or not they are `0.`.
    /// @param matrix The matrix to be checked.
    /// @param tolerance The number above which the modulus of an element is not considered to be `0.`.
    /// @return `true` if the matrix is in upper Hessenberg form within `tolerance`, `false` otherwise.
    /// @pre `matrix` must be finite as per `hmeigens::isFinite()`.
    /// @pre `tolerance` must be finite as per `std::isfinite()`. It must also be non-negative, as the function performs no checks on it. Giving a negative value will result in a meaningless trivially `false` result (except for `1x1` and `2x2` matrices, for which the function always returns `true`).
    [[nodiscard]] bool isUpperHessenberg(const SquareMatrix& matrix, const Scalar tolerance);
}

#endif
