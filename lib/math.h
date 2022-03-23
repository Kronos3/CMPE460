
#ifndef CMPE460_MATH_H
#define CMPE460_MATH_H

#include <fw.h>

/**
 * Cast a U16 vector into an F64 vector
 * @param dest Destination vector
 * @param src Source vector
 * @param n length of source/destination
 */
void math_cast(F64* dest, const U16* src, U32 n);

#define MATH_CONVOLVE_N(x_n, h_n) ((x_n) + (h_n) - 1)

/**
 * Linear convolution of h() through x()
 * Place results in y.
 * @param y Destination vector at least MATH_CONVOLVE_N long
 * @param x vector to convolve through
 * @param h vector to convolve
 * @param x_n Length of x
 * @param h_n Length of h
 */
void math_convolve(
        F64 y[],
        const F64 x[],
        const F64 h[],
        U32 x_n,
        U32 h_n);

/**
 * Apply a 1D gaussian smoothing operator
 * @param y destination gaussian operator
 * @param n length of y (number of samples)
 * @param sigma Gaussian operator sigma
 */
void math_gaussian_smoothing(F64 y[], U32 n, F64 sigma);

/**
 * Compute the X-gradient of vector
 * @param dx Final gradient (length n - 1)
 * @param x Input vector (length n)
 * @param n Length of x
 */
void math_gradient(F64 dx[], const F64 x[], U32 n);

/**
 * Apply a binary threshold
 * @param y Threshold vector, 0 if lower, 1 if higher
 * @param x input vector
 * @param threshold threshold to apply
 * @param n number of points to threshold
 */
void math_threshold(U8 y[], const F64 x[], F64 threshold, U32 n);

#endif //CMPE460_MATH_H
