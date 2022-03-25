#ifndef CMPE460_UART_H
#define CMPE460_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>

#ifndef __uart_LINKED__
//#error "You need to link 'uart' to use this header"
#endif

#ifdef __BCM2835__
#include <drv/rpi/uart.h>
#error "uart is not supported on BCM2835 yet!"
#elif defined(__MSP432P4__)
#include <drv/msp432p4/uart.h>
#endif

/**
 * Initialize the UART to an operating state
 * @param id UART device to initialize
 * @param baud_rate baud rate to initialize device to
 */
void uart_init(uart_t id, U32 baud_rate);

/**
 * Read a byte from the UART Rx line
 * @param id device to read from
 * @return byte read from UART
 */
U8 uart_getchar(uart_t id);

/**
 * Write a character to the UART Tx line
 * @param id device to transmit over
 * @param ch character write
 */
void uart_putchar(uart_t id, char ch);

/**
 * Write a standard C string to the UART Tx line
 * @param id device to transmit string over
 * @param ptr_str string to write
 */
void uart_put(uart_t id, const char* ptr_str);

/**
 * Write an ar
 * @param id
 * @param ptr
 * @param len
 */
void uart_write(uart_t id, const char* ptr, U32 len);

/**
 * Read a line (handling
 * @param id
 * @param buf
 * @param len
 * @return
 */
char* uart_getline(uart_t id, char buf[], U32 len);

/**
 * Check to see whether there is data available in the UART
 * @param id device to check
 * @return TRUE if data is available, FALSE otherwise
 */
bool_t uart_peek(uart_t id);

#ifdef __cplusplus
}
#endif

#endif // CMPE460_UART_H
