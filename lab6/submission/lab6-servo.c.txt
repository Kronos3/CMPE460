#include <drv/uart.h>
#include <drv/msp432p401r/pwm.h>
#include <stdlib.h>
#include <drv/pwm.h>

int main()
{
    uart_init(UART_USB, 115200);

    // P2.4
    PwmPin forward = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_1
    };

    pwm_init(forward, 50.0);
    pwm_set(forward, 0.0);
    pwm_start(forward);

    while(1)
    {
        char input[32];
        uart_getline(UART_USB, input, sizeof input);

        double input_d = strtod(input, NULL);
        pwm_set(forward, input_d);
    }
}
