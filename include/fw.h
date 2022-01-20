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
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

/**
 * Handle assertion failures
 * Hang the cpu and flash all of the LEDs
 * @param file File where assertion failure occurred
 * @param line Line of assertion failure
 * @param expr_str Asserted expression failure
 * @param nargs number of arguments to print
 * @param ... arguments to print
 */
__attribute__((noreturn)) void fw_assertion_failure(const char* file, U32 line, const char* expr_str, U32 nargs, ...);

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT_ARGUMENTS(...) ELEVENTH_ARGUMENT(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define FW_ASSERT(expr, ...) do {                \
    if (!(expr)) fw_assertion_failure(__FILE__, __LINE__, #expr, COUNT_ARGUMENTS(__VA_ARGS__), ##__VA_ARGS__);   \
} while(0)

#endif //CMPE460_FW_H
