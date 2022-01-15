#ifndef CMPE460_UART_H
#define CMPE460_UART_H

#include <fw.h>

/**
 * Initialize the UART to an operating state
 */
void uart0_init();

/**
 * Read a byte from the UART Rx line
 * @return
 */
U8 uart0_getchar();

/**
 * Write a character to the UART Tx line
 * @param ch character write
 */
void uart0_putchar(char ch);

/**
 * Write a standard C string to the UART Tx line
 * @param ptr_str string to write
 */
void uart0_put(char* ptr_str);

#endif // CMPE460_UART_H
