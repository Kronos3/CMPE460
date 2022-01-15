/*
 * CMPE460 Framework header
 * All global definitions and commonly used functionalities
 * Author: Andrei Tumbar
 */

#ifndef CMPE460_FW_H
#define CMPE460_FW_H

#include <stdint.h>

typedef signed char I8;
typedef unsigned char U8;
typedef signed short I16;
typedef unsigned short U16;
typedef signed int I32;
typedef unsigned long U32;

typedef float F32;
typedef double F64;

typedef enum
{
    FALSE,
    TRUE
} bool_t;

#define COMPILE_ASSERT(expr, name) typedef char __compile_assert_##name[(expr) ? 0 : -1]

#define ABS(x) ((x) < 0) ? (-(x)) : (x)

#endif //CMPE460_FW_H
