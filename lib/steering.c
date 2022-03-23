#include "steering.h"

SteeringParams steering_params;
bool_t steering_configured = FALSE;

void steering_init(const SteeringParams* params)
{
    FW_ASSERT(params);
    FW_ASSERT(!steering_configured);

    steering_params = *params;
    pwm_init(steering_params.servo_pin, steering_params.servo_pwm_freq);
    steering_configured = TRUE;
}

void steering_start(void)
{
    pwm_start(steering_params.servo_pin);
    steering_set(0.0);
}

void steering_set(F64 position)
{
    // Clamp the values to the minimum and maximum
    position = FW_MIN(position, 1.0);
    position = FW_MAX(position, -1.0);

    // Convert the normalized value into a PWM value
    F64 position_normal = (position + 1.0) / 2.0;
    F64 pwm_range = steering_params.right_pwm - steering_params.left_pwm;
    F64 duty;
    if (steering_params.left_pwm < steering_params.right_pwm)
    {
        duty = position_normal * pwm_range + steering_params.left_pwm;

        // Clamp this duty cycle
        duty = FW_MIN(duty, steering_params.right_pwm);
        duty = FW_MAX(duty, steering_params.left_pwm);
    }
    else
    {
        duty = position_normal * pwm_range + steering_params.right_pwm;

        // Clamp this duty cycle
        duty = FW_MIN(duty, steering_params.left_pwm);
        duty = FW_MAX(duty, steering_params.right_pwm);
    }

    pwm_set(steering_params.servo_pin, duty);
}
