/*
Title: Lab2 Part 3
Description: UART Echo program
Authors: Andrei Tumbar
Date: 1/15/22


All rights reserved.
*/

#include<stdio.h>
#include "uart.h"            // you will need to create this file

//default baud rate
#define BAUD_RATE 9600

//change this to modify the max. permissible length of a sentence
#define CHAR_COUNT 10

int main()
{
    uart0_init();

    // write this string to the UART
    uart0_put("\r\n IDE Lab: Instructed by Prof. Louis Beato");    /*Transmit this through UART*/

    U32 character_count = 0;
    char inputted_lines[CHAR_COUNT];
    while (1)
    {
        if (character_count == 0)
        {
            // send carriage return/line feed to uart
            uart0_put("\r\n");
        }

        uart0_put("input string: ");

        while (character_count < CHAR_COUNT)
        {
            // read a character from the UART
            char ch = uart0_getchar();
            inputted_lines[character_count++] = ch;

            // write the character to the UART
            uart0_putchar(ch);

            // if "RETURN" character, break
            if (ch == '\r')
            {
                break;
            }
        }

        // print the line back to the user
        uart0_put("\r\n");
        for (U32 i = 0; i < character_count; i++)
        {
            uart0_putchar(inputted_lines[i]);
        }

        // reset character count
        character_count = 0;
    }

}
