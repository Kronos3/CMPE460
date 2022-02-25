#ifndef CMPE460_PWM_H
#define CMPE460_PWM_H

#include <fw.h>

#ifndef __pwm_LINKED__
#error "You need to link 'pwm' to use this header"
#endif

#include <gpio.h>

typedef enum
{
    PWM_0,
    PWM_1,
    PWM_2,
    PWM_3,
} pwm_t;

typedef enum
{
    PWM_PRESCALE_1,
    PWM_PRESCALE_2,
    PWM_PRESCALE_3,
    PWM_PRESCALE_4,
    PWM_PRESCALE_5,
    PWM_PRESCALE_6,
    PWM_PRESCALE_7,
    PWM_PRESCALE_8,
} pwm_prescaler_t;

COMPILE_ASSERT(PWM_PRESCALE_8 == 0x7, pwm_check_psc_enum);

typedef enum
{
    PWM_PIN_0,
    PWM_PIN_1,
    PWM_PIN_2,
    PWM_PIN_3,
    PWM_PIN_4,
    PWM_PIN_N,
} pwm_pin_t;

/**
 * Initialize the TimerA to run at a specific frequency
 * @param pwm pwm channel to initialize
 * @param prescaler_1 clock first prescaler to use
 * @param prescaler_2 clock first prescaler to use
 * @param rate frequency of the pwm
 * @return the gpio pin this pwm channel maps to
 */
void pwm_init(pwm_t pwm,
              pwm_prescaler_t prescaler_1,
              pwm_prescaler_t prescaler_2,
              F32 rate);

/**
 * Initialize own of the output pins on a pwm channel
 * @param pwm pwm channel where pin lies
 * @param pin pin to initialize
 */
void pwm_init_pin(pwm_t pwm, pwm_pin_t pin);

/**
 * Set the duty cycle on a pwm channel
 * (able to do while running)
 * @param pwm pwm channel to set duty cycle on
 * @param pwm_pin_t pwm channel to set duty cycle on
 * @param duty duty cycle (between 0.0 - 1.0)
 */
void pwm_set(pwm_t pwm, pwm_pin_t pin, F64 duty);

/**
 * Start the pulse on a pwm channel
 * @param pwm channel to start pulse on
 * @param pin pin to start pulse on
 */
void pwm_start(pwm_t pwm, pwm_pin_t pin);

/**
 * Stop the pulse on a pwm channel
 * @param pwm channel to stop pulse on
 */
void pwm_stop(pwm_t pwm);

#endif //CMPE460_PWM_H
