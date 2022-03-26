#include <math.h>
#include <string.h>
#include "math.h"

void math_cast(F64* dest, const U16* src, U32 n)
{
    for (U32 i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
}

void math_convolve(
        F64 y[],
        const F64 x[],
        const F64 h[],
        U32 x_n,
        U32 h_n)
{
    FW_ASSERT(y);
    FW_ASSERT(x);
    FW_ASSERT(h);
    FW_ASSERT(x_n);
    FW_ASSERT(h_n);

    memset(y, 0, MATH_CONVOLVE_N(x_n, h_n) * sizeof(F64));

    I32 i, j, h_start, x_start, x_end;
    for (i = 0; i < MATH_CONVOLVE_N(x_n, h_n); i++)
    {
        x_start = FW_MAX(0, i - h_n + 1);
        x_end = FW_MIN(i + 1, x_n);
        h_start = FW_MIN(i, h_n - 1);
        for (j = x_start; j < x_end; j++)
        {
            y[i] += h[h_start--] * x[j];
        }
    }
}

static F64 gaussian_1d(I32 x, F64 sigma)
{
    F64 coeff = 1.0 / (sqrt(2.0 * M_PI) * sigma);
    F64 p = pow(M_E, (-x * x) / (2 * sigma * sigma));
    return coeff * p;
}

void math_gaussian_smoothing(F64 y[], U32 n, F64 sigma)
{
    FW_ASSERT(y);
    FW_ASSERT(n % 2 == 1, n); // make sure n is odd
    I32 s = (I32) n / 2;

    for (I32 i = 0; i < n; i++)
    {
        y[i] = gaussian_1d(i - s, sigma);
    }
}

void math_gradient(F64 dx[], const F64 x[], U32 n)
{
    for (U32 i = 0; i < n - 1; i++)
    {
        dx[i] = x[i + 1] - x[i];
    }
}

void math_threshold(U8 y[], const F64 x[], F64 threshold, U32 n)
{
    for (U32 i = 0; i < n; i++)
    {
        if (x[i] >= 0)
        {
            y[i] = x[i] >= threshold;
        }
        else
        {
            y[i] = x[i] <= -threshold;
        }
    }
}
