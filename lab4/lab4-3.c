/*
Title: Lab4
Description: Bluetooth chatroom
Authors: Andrei Tumbar
Date: 1/19/22


All rights reserved.
*/

#include "uart.h"

#define BUF_SIZE 64
#define BAUD_RATE 9600

int main()
{
    // Allocate enough buffers to communicate over both UARTs
    static char uart_buf[UART_INVALID][BUF_SIZE];
    static const char uart_prompts[UART_INVALID][10] = {"PC> ", "Phone> "};
    static I32 uart_pos[UART_INVALID] = {0};

    uart_init(UART_USB, BAUD_RATE);
    uart_init(UART_BT, BAUD_RATE);

    while(1)
    {
        for (uart_t uart = 0; uart < UART_INVALID; uart++)
        {
            // Check if there is any available data on this UART
            if (uart_peek(uart))
            {
                char ch = uart_getchar(uart);
                if (ch == '\r' || ch == '\n') // handle all CR, LF and CRLF line endings
                {
                    // End of line
                    if (uart == UART_USB) uart_put(UART_USB, "\r\n");

                    // Print the final chat
                    // Both sides should see who the message is from
                    uart_put(UART_USB, uart_prompts[uart]);
                    uart_put(UART_BT, uart_prompts[uart]);

                    // Send the data to the other user
                    uart_write(UART_USB, uart_buf[uart], uart_pos[uart]);
                    uart_write(UART_BT, uart_buf[uart], uart_pos[uart]);

                    // Reset the input
                    uart_put(UART_USB, "\r\n");
                    uart_put(UART_BT, "\r\n");
                    uart_pos[uart] = 0;
                }
                else if (ch == 0x7F) // handle DEL ascii
                {
                    if (uart_pos[uart] == 0)
                    {
                        // No more characters to delete
                        continue;
                    }

                    uart_put(uart, "\b \b");
                    uart_pos[uart]--;
                }
                else // handle normal characters by adding them to the buffer
                {
                    // Don't overfill the buffers
                    // This will truncate the line until you press 'ENTER'
                    if (uart_pos[uart] >= BUF_SIZE) continue;

                    // Add the character to the buffer and increment the position
                    uart_buf[uart][uart_pos[uart]++] = ch;

                    // Print the character to the screen, so we can see what we're typing
                    // We don't want this with bluetooth.
                    // Bluetooth serial apps already have a prompt buffer
                    if (uart != UART_BT) uart_putchar(uart, ch);
                }
            }
        }
    }
}
