#include <lib/dc.h>
#include <drv/uart.h>
#include <stdlib.h>
#include <lib/uartlib.h>

#define DC_FREQ (10000)

DcParam left = {
        // P2.4
        .backward={
                MSP432_PWM_0,
                MSP432_PWM_PIN_1
        },
        // P2.5
        .forward={
                MSP432_PWM_0,
                MSP432_PWM_PIN_2
        },
        .enable = GPIO_PIN(3, 6),
        .base_frequency=DC_FREQ
};
DcParam right = {
        // P2.6
        .forward={
                MSP432_PWM_0,
                MSP432_PWM_PIN_3
        },
        // P2.7
        .backward={
                MSP432_PWM_0,
                MSP432_PWM_PIN_4
        },
        .enable = GPIO_PIN(3, 7),
        .base_frequency=DC_FREQ
};

int main()
{
    uart_init(UART_USB, 115200);

    dc_cfg(DC_0, &left);
    dc_cfg(DC_1, &right);
    dc_init();
    dc_start();

    uprintf("Ready\r\n");

    while(1)
    {
        char input[32];
        uart_getline(UART_USB, input, sizeof input);

        double input_d = strtod(input, NULL);
        dc_set(DC_0, input_d);
        dc_set(DC_1, input_d);
    }
}

