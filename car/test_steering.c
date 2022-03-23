#include <lib/steering.h>
#include <drv/uart.h>
#include <stdlib.h>

#define SERVO_FREQ (50.0)

int main()
{
    SteeringParams steering = {
            // P2.4
            .servo_pin={
                    MSP432_PWM_0,
                    MSP432_PWM_PIN_1
            },
            .servo_pwm_freq=SERVO_FREQ,
            .left_pwm=0.035,
            .right_pwm=0.10
    };

    uart_init(UART_USB, 115200);

    steering_init(&steering);
    steering_start();

    while(1)
    {
        char input[32];
        uart_getline(UART_USB, input, sizeof input);

        double input_d = strtod(input, NULL);
        steering_set(input_d);
    }
}
