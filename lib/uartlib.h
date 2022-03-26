#ifndef CMPE460_UARTLIB_H
#define CMPE460_UARTLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <drv/uart.h>

// Defines which UART to use for printf
#ifndef UART_IO
#define UART_IO UART_USB
#endif

I32 uprintf(const char* fmt, ...);
I32 ufprintf(uart_t uart, const char* fmt, ...);

I32 uvfprintf(
        uart_t uart,
        const char* format,
        va_list args);

typedef enum
{
    DEBUG_HEX_16,
    DEBUG_HEX_32,
    DEBUG_DEC_8,
    DEBUG_DEC_16,
    DEBUG_DEC_32,
    DEBUG_FLOAT_LARGE,
    DEBUG_FLOAT_SMALL,
} debug_t;

void udebug_dump(const void* data, debug_t type, U32 screen_n, U32 n);

#ifdef __cplusplus
}
#endif

#endif //CMPE460_UARTLIB_H
