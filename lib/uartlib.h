#ifndef CMPE460_UARTLIB_H
#define CMPE460_UARTLIB_H

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

#endif //CMPE460_UARTLIB_H
