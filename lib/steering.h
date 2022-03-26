
#ifndef CMPE460_STEERING_H
#define CMPE460_STEERING_H

#include <fw.h>
#include <drv/pwm.h>

typedef struct
{
    PwmPin servo_pin;       //!< PWM pin to control the servo position
    F64 left_pwm;           //!< Left PWM limit
    F64 right_pwm;          //!< Right PWM limit
    F64 servo_pwm_freq;     //!< PWM base frequency
} SteeringParams;

void steering_init(const SteeringParams* params);

/**
 * Steer the car
 * Values outside the range will be truncated
 * @param position -1.0 left to 1.0 right
 */
void steering_set(F64 position);

/**
 * Starts the PWM signal on the servo
 * Aligns the steering to center
 */
void steering_start(void);
void steering_stop(void);

#endif //CMPE460_STEERING_H
