#include <drv/uart.h>
#include <drv/pwm.h>

int main(void)
{
    uart_init(UART_USB, 115200);

    PwmPin pin = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_1
    };
    pwm_init(pin, 10000);

    // 10 kHz 20% duty cycle on pin P2.4
    pwm_set(pin, 0.20);
    pwm_start(pin);

    while (TRUE);
}
