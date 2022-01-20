/*
Title: Lab4 part 2
Description: Bluetooth control of LEDs
Authors: Andrei Tumbar
Date: 1/19/22


All rights reserved.
*/

#include <uart.h>
#include <led.h>

#define BAUD_RATE 9600

int main()
{
    uart_init(UART_USB, BAUD_RATE);
    uart_init(UART_BT, BAUD_RATE);

    led_init();

    uart_put(UART_BT, "READY\r\n");
    uart_put(UART_USB, "READY\r\n");

    while(1)
    {
        uart_t device_with_data = UART_INVALID;
        for (uart_t device = 0; device < UART_INVALID; device++)
        {
            if (uart_peek(device))
            {
                device_with_data = device;
                break;
            }
        }

        if (device_with_data != UART_INVALID)
        {
            char input_ch = uart_getchar(device_with_data);
            I32 input = input_ch - '0';
            if (input < 0 || input > 3)
            {
                uart_put(device_with_data, "Invalid input\r\n");
                continue;
            }

            static const U32 lights[] = {
                    0,
                    LED2_RED,
                    LED2_BLUE,
                    LED2_GREEN
            };

            static const char* light_name[] = {
                    "OFF\r\n",
                    "RED\r\n",
                    "BLUE\r\n",
                    "GREEN\r\n",
            };

            // Set the color on the LED
            led_set(lights[input]);

            // Tell the UART which color it is
            uart_put(device_with_data, light_name[input]);
        }
    }
}