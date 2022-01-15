/*
Title: Lab2 Part 2
Description: Initial UART lab
Authors: Andrei Tumbar
Date: 1/15/22


All rights reserved.
*/

#include<stdio.h>
#include <driverlib.h>
#include "msp.h"
#include "uart.h"            // you will need to create this file

//default baud rate
#define BAUD_RATE 9600

//change this to modify the max. permissible length of a sentence
#define CHAR_COUNT 10

void put(char* put_str);

int main()
{
    uart0_init();
    put("\r\n IDE Lab2: UART's are awesome!");    /*Transmit this through UART*/
}

void put(char* ptr_str)
{
    while (*ptr_str)
    {
        uart0_putchar(*ptr_str++);
    }
}
