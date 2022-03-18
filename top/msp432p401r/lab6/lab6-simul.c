#include <drv/msp432p401r/pwm.h>
#include <lib/dc.h>
#include <instr.h>

int main(void)
{
    // P2.4
    PwmPin left = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_1
    };
    // P2.6
    PwmPin right = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_3
    };

    // P5.6
    PwmPin servo = {
            MSP432_PWM_2,
            MSP432_PWM_PIN_1
    };

    pwm_init(left, 10000);
    pwm_init(right, 10000);
    pwm_init(servo, 50);

    pwm_start(left);
    pwm_start(right);
    pwm_start(servo);

    pwm_set(left, 0.3);
    pwm_set(right, 0.3);
    pwm_set(servo, 0.075);

    while (TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}
