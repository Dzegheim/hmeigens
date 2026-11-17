/// @file
/// @brief A collection of functions that verify the properties of a given matrix.
// Declarations (hmeigens):
//     - isFinite(const SquareMatrix&);
//     - getTolerance(const SquareMatrix&);
//     - isUpperHessenberg(const SquareMatrix&, const Scalar);
//     - isUpperTriangular(const SquareMatrix&, const Scalar).
#ifndef HMEIGENS_PREDICATES_HPP
#define HMEIGENS_PREDICATES_HPP

#include "hmeigens/square_matrix.hpp"
#include "hmeigens/scalar.hpp"

namespace hmeigens {

    /// @brief Checks whether all elements of the matrix satisfy `std::isfinite()` for both their real and imaginary parts.
    /// @param matrix The matrix to be checked.
    /// @return `true` if all elements of the matrix are finite, `false` otherwise.
    [[nodiscard]] bool isFinite(const SquareMatrix& matrix);

    /// @brief Computes the tolerance that is used by predicates to assess if numbers verify properties.
    ///
    /// The Francis QR algorithm performs several operations, each introducing roundings. The tolerance adopted here comes from the bounds provided in *Matrix Computations* (Golub & Van Loan, 4th edition, 2013).
    /// In particular the statements that matter here are:
    /// - Section 7.4.3 **The Hessenberg Reduction**: after reducing a matrix \f(\mathbf{M}\f) to Hessenberg form, this form is similar to (and thus has the same eigenvalues as) \f(\mathbf{M}+\mathbf{P}\f), where \f(\mathbf{P}\f) is a perturbation of \f(\mathbf{M}\f) whose norm is \f(\left\|\mathbf{P}\right\|_F\leq c\cdot n^2\cdot u\cdot\left\|\mathbf{M}\right\|_F\f), with \f(c\f) a small constant, \f(n\f) the size of the matrix \f(\mathbf{M}\f), \f(u\f) the unit roundoff (i.e. half of `hmeigens::epsilon`), and \f(\left\|\cdot\right\|_F\f) the Frobenius norm (`hmeigens::SquareMatrix::norm`, also \f(\left\|\cdot\right\|\f));
    /// - Section 7.5.6 **QR algorithm**: for the whole process the norm of the perturbation is of the same order of magnitude as \f(u\cdot\left\|\mathbf{M}\right\|_2\f), where \f(\left\|\cdot\right\|_2\f) is the \f(2\f)-norm, which satisfies \f(\left\|\mathbf{M}\right\|_2 \leq \left\|\mathbf{M}\right\|_F \leq \sqrt{n}\left\|\mathbf{M}\right\|_2\f) (*ibid.*, 2.3.2).
    ///
    /// In this project the Frobenius norm is used instead of the \f(2\f)-norm due to it being easier to compute, and the two are equivalent in practice. The tolerance for this project is therefore chosen as:
    /// \f[
    /// k\cdot n^2\cdot\left\|\mathbf{M}\right\|\cdot\varepsilon\ ,
    /// \f]
    /// where \f(\varepsilon=2u\f) is `hmeigens::epsilon`, while \f(k\f) is `hmeigens::toleranceK`.
    /// The value \f(n^2\f) is included in the tolerance for the whole process since it is part of the Hessenberg reduction bound, and no information is provided about its influence on the whole QR algorithm.
    /// The value of the constant incorporates the factor of \f(2\f) between \f(u\f) and \f(\varepsilon\f), as well as \f(c\f).
    ///
    /// This tolerance is used to determine, for example, when a number is `0.` with respect to the matrix to verify triangularity, or any other property described by the other functions of `predicates.hpp`.
    /// @param matrix The matrix to compute the tolerance of.
    /// @return The tolerance computed as \f(k\cdot n^2\cdot\left\|\mathbf{M}\right\|\cdot\varepsilon\f).
    /// @pre `matrix` must be finite as per `hmeigens::isFinite()`.
    /// @sa hmeigens::toleranceK
    /// @sa HMEIGENS_TOLERANCE_K
    [[nodiscard]] Scalar getTolerance(const SquareMatrix& matrix);

    /// @brief Checks whether a matrix is in upper Hessenberg form within a given tolerance.
    ///
    /// A matrix is upper Hessenberg when it is in the form
    /// \f[
    /// \mathbf{M} =
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
    /// @pre `tolerance` must be finite as per `std::isfinite()`. It must also be non-negative, as the function performs no checks on it. Giving a negative value will result in a meaningless trivially `false` result (except for \f(1\times1\f) and \f(2\times2\f) matrices, for which the function always returns `true`).
    /// @sa hmeigens::isUpperTriangular
    [[nodiscard]] bool isUpperHessenberg(const SquareMatrix& matrix, const Scalar tolerance);

    /// @brief Checks whether a matrix is in upper triangular form within a given tolerance.
    ///
    /// A matrix is upper triangular when it is in the form
    /// \f[
    /// \mathbf{M} = \begin{pmatrix}
    ///    m_{1,1} & m_{1,2} & \cdots  & m_{1,n}
    /// \\ 0       & m_{2,2} & \cdots  & m_{2,n}
    /// \\ \vdots  & \ddots  & \ddots  & \vdots
    /// \\ 0       & \cdots  & 0       & m_{n,n}
    /// \end{pmatrix}\ .
    /// \f]
    /// This function compares the moduli of all elements below the diagonal with the given tolerance, to determine whether or not they are `0.`.
    /// @param matrix The matrix to be checked.
    /// @param tolerance The number above which the modulus of an element is not considered to be `0.`.
    /// @return `true` if the matrix is in upper triangular form within `tolerance`, `false` otherwise.
    /// @pre `matrix` must be finite as per `hmeigens::isFinite()`.
    /// @pre `tolerance` must be finite as per `std::isfinite()`. It must also be non-negative, as the function performs no checks on it. Giving a negative value will result in a meaningless trivially `false` result (except for \f(1\times1\f) matrices, for which the function always returns `true`).
    /// @sa hmeigens::isUpperHessenberg
    [[nodiscard]] bool isUpperTriangular(const SquareMatrix& matrix, const Scalar tolerance);
}

#endif
