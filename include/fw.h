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

typedef signed char I8;
typedef unsigned char U8;
typedef signed short I16;
typedef unsigned short U16;
typedef signed int I32;
typedef unsigned long U32;

typedef float F32;
typedef double F64;

// Pointer sized
typedef U32 PXX;

#define COMPILE_ASSERT(expr, name) typedef char __compile_assert_##name[(expr) ? 0 : -1]

// Check that the compiler agrees with our defines
COMPILE_ASSERT(sizeof(I8) == 1, sizeof_i8);
COMPILE_ASSERT(sizeof(U8) == 1, sizeof_u8);
COMPILE_ASSERT(sizeof(I16) == 2, sizeof_i16);
COMPILE_ASSERT(sizeof(U16) == 2, sizeof_u16);
COMPILE_ASSERT(sizeof(I32) == 4, sizeof_i32);
COMPILE_ASSERT(sizeof(U32) == 4, sizeof_u32);
COMPILE_ASSERT(sizeof(F32) == 4, sizeof_f32);
COMPILE_ASSERT(sizeof(F64) == 8, sizeof_f64);

#ifndef UINT32_MAX
#define UINT32_MAX (0xFFFFFFFF)
#endif

#ifndef INT32_MAX
#define INT32_MAX (0x7FFFFFFF)
#endif

typedef enum
{
    FALSE,
    TRUE
} bool_t;

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

#define FW_ABS(x_) ((x_) < 0 ? -(x_) : (x_))

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#define SQR(x) ((x)*(x))

/**
 * Mask all interrupts on the CPU
 */
#define DISABLE_INTERRUPTS() __asm__("cpsid if")

/**
 * Unmask all interrupts (that were previously masked)
 */
#define ENABLE_INTERRUPTS() __asm__("cpsie if")

/**
 * Sets a debugger breakpoint
 */
#define BREAKPOINT() __asm__("bkpt")

/**
 * Enters low power sleep mode waiting for interrupt (WFI instruction)
 * Processor waits for next interrupt
 */
#define WAIT_FOR_INTERRUPT() __asm__("WFI")

#endif //CMPE460_FW_H
#endif //__TUMBAR_RTOS__
