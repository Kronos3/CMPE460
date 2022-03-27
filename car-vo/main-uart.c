#include <drv/uart.h>
#include <drv/msp432p4/led.h>
#include <drv/msp432p4/switch.h>
#include <lib/uartlib.h>
#include "common.h"

int main()
{
//    uart_init(UART_GPIO, 115200);
    uart_init(UART_USB, 115200);
    led_init(LED2_ALL);

    car_init();
    switch_init(SWITCH_ALL, SWITCH_INT_PRESS, car_toggle);

    // Turn off the motors until the switch is pressed
    car_set(FALSE);

    Command cmd;

    uprintf("Ready\r\n");

    while (1)
    {
        // Wait for two bytes from the pi
        cmd.opcode = uart_getchar(UART_GPIO);
        cmd.value = uart_getchar(UART_GPIO);

        dispatch_command(&cmd);
    }
}
