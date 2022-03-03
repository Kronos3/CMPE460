#ifndef CMPE460_PWM_H
#define CMPE460_PWM_H

#include <fw.h>

#ifndef __pwm_LINKED__
#error "You need to link 'pwm' to use this header"
#endif

#include <drv/gpio.h>

#ifdef __RPI_ZERO__
#include "rpi_zero/pwm.h"
#elif defined(__MSP432P401R__)
#include "msp432p401r/pwm.h"
#endif

/**
 * Initialize the TimerA to run at a specific frequency
 * @param pwm pwm and pin to initialize
 * @param rate frequency of the pwm
 * @return the gpio pin this pwm channel maps to
 */
void pwm_init(PwmPin pwm, F64 rate);

/**
 * Set the duty cycle on a pwm channel
 * (able to do while running)
 * @param pwm pwm channel to set duty cycle on
 * @param pwm_pin_t pwm channel to set duty cycle on
 * @param duty duty cycle (between 0.0 - 1.0)
 */
void pwm_set(PwmPin pwm, F64 duty);

/**
 * Start the pwm signal on a PWM pin
 * @param pwm pwm pin to start signal on
 */
void pwm_start(PwmPin pwm);

/**
 * Stop the pwm signal on a PWM pin
 * Only use this if you want to stop the hardware timer running the pwm
 * Otherwise use pwm_set(pwm, 0.0)
 * @param pwm pwm pin to stop signal on
 */
void pwm_stop(PwmPin pwm);

#endif //CMPE460_PWM_H
