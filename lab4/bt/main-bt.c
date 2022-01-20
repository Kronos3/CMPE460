/*
Title: Lab4
Description: Bluetooth
Authors: Andrei Tumbar
Date: 1/15/22


All rights reserved.
*/

#include "uart.h"            // you will need to create this file

#define BAUD_RATE 9600      //default baud rate

int main()
{
    uart_init(UART_USB, BAUD_RATE);
    uart_init(UART_BT, BAUD_RATE);

    uart_put(UART_USB, "Hello world\r\n");
    uart_put(UART_BT, "Hello from the bluetooth\r\n");
}
