/*
Title: Lab2 Part 3
Description: UART Echo program
Authors: Andrei Tumbar
Date: 1/15/22


All rights reserved.
*/

#include <drv/uart.h>

//default baud rate
#define BAUD_RATE 9600

//change this to modify the max. permissible length of a sentence
#define CHAR_COUNT 10

int main()
{
    uart_init(UART_USB, BAUD_RATE);

    // write this string to the UART
    uart_put(UART_USB, "\r\n IDE Lab: Instructed by Prof. Louis Beato");    /*Transmit this through UART*/

    U32 character_count = 0;
    char inputted_line[CHAR_COUNT];
    while (1)
    {
        // Send the prompt to the user
        uart_put(UART_USB, "\r\ninput string: ");

        while (character_count < CHAR_COUNT)
        {
            // read a character from the UART
            char ch = uart_getchar(UART_USB);
            inputted_line[character_count++] = ch;

            // write the character to the UART
            uart_putchar(UART_USB, ch);

            // if "RETURN" character, break
            if (ch == '\r')
            {
                break;
            }
        }

        // print the line back to the user
        uart_put(UART_USB, "\r\n");
        for (U32 i = 0; i < character_count; i++)
        {
            uart_putchar(UART_USB, inputted_line[i]);
        }

        // reset character count
        character_count = 0;
    }

}
