/*
Title: Lab2 Part 2
Description: Initial UART lab
Authors: Andrei Tumbar
Date: 1/15/22


All rights reserved.
*/

#include <uart.h>

//default baud rate
#define BAUD_RATE 9600

//change this to modify the max. permissible length of a sentence
#define CHAR_COUNT 10

int main()
{
    uart_init(UART_USB, BAUD_RATE);
    uart_put(UART_USB, "\r\n IDE Lab2: UART's are awesome!");    /*Transmit this through UART*/
}
