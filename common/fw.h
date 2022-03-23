/*
 * CMPE460 Framework header
 * All global definitions and commonly used functionalities
 * Author: Andrei Tumbar
 */

#ifdef __TUMBAR_RTOS__
#include <global.h>
#else
#ifndef CMPE460_FW_H
#define CMPE460_FW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char I8;
typedef unsigned char U8;
typedef signed short I16;
typedef unsigned short U16;
typedef signed int I32;
typedef unsigned int U32;

typedef float F32;
typedef double F64;

#define COMPILE_ASSERT(expr, name) enum { __compile_assert_##name = 1 / ((expr) ? 1 : 0) }

// Check that the compiler agrees with our defines
COMPILE_ASSERT(sizeof(I8) == 1, sizeof_i8);
COMPILE_ASSERT(sizeof(U8) == 1, sizeof_u8);
COMPILE_ASSERT(sizeof(I16) == 2, sizeof_i16);
COMPILE_ASSERT(sizeof(U16) == 2, sizeof_u16);
COMPILE_ASSERT(sizeof(I32) == 4, sizeof_i32);
COMPILE_ASSERT(sizeof(U32) == 4, sizeof_u32);
COMPILE_ASSERT(sizeof(F32) == 4, sizeof_f32);
COMPILE_ASSERT(sizeof(F64) == 8, sizeof_f64);

#if __SIZEOF_LONG_LONG__ == 8
typedef signed long long int I64;
typedef unsigned long long int U64;
COMPILE_ASSERT(sizeof(I64) == 8, sizeof_i64);
COMPILE_ASSERT(sizeof(U64) == 8, sizeof_u64);
#endif

// Pointer sized
#if __SIZEOF_POINTER__ == 4
typedef U32 PXX;
#elif __SIZEOF_POINTER__ == 8
typedef U64 PXX;
#endif

#ifndef UINT32_MAX
#define UINT32_MAX (0xFFFFFFFF)
#endif

#ifndef INT32_MAX
#define INT32_MAX (0x7FFFFFFF)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX (0xFFFF)
#endif

#ifndef INT16_MAX
#define INT16_MAX (0x7FFF)
#endif

typedef enum
{
    FALSE,
    TRUE
} bool_t;

#define FW_ABS(x) ((x) < 0) ? (-(x)) : (x)
#define FW_MIN(a, b) ((a) < (b)) ? (a) : (b)
#define FW_MAX(a, b) ((a) > (b)) ? (a) : (b)

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

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#define SQR(x) ((x)*(x))

#ifndef offsetof
#ifdef __builtin_offsetof
#define offsetof __builtin_offsetof
#else
#define offsetof(st_, f_) ((PXX)&(((st_ *)0)->f_))
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif //CMPE460_FW_H
#endif //__TUMBAR_RTOS__
