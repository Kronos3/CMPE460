#include <uart.h>
#include <pwm.h>

int main(void)
{
    uart_init(UART_USB, 115200);

    pwm_init(PWM_0, PWM_PRESCALE_1, PWM_PRESCALE_4, 10000);
    pwm_init_pin(PWM_0, PWM_MODE_RESET_SET, PWM_PIN_1);

    // 10 kHz 20% duty cycle on pin P2.4
    pwm_set(PWM_0, PWM_PIN_1, 0.20);
    pwm_start(PWM_0, PWM_PIN_1);

    while (TRUE);
}
