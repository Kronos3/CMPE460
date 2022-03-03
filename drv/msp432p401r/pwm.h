#ifndef CMPE460_MSP432_PWM_H
#define CMPE460_MSP432_PWM_H

typedef struct Msp432_PwmPin_prv PwmPin;

typedef enum
{
    MSP432_PWM_0,
    MSP432_PWM_1,
    MSP432_PWM_2,
    MSP432_PWM_3,
} msp432_pwm_t;

typedef enum
{
    MSP432_PWM_PIN_0,
    MSP432_PWM_PIN_1,
    MSP432_PWM_PIN_2,
    MSP432_PWM_PIN_3,
    MSP432_PWM_PIN_4,
    MSP432_PWM_PIN_N,
} msp432_pwm_pin_t;

typedef enum
{
    MSP432_PWM_PRESCALE_1,
    MSP432_PWM_PRESCALE_2,
    MSP432_PWM_PRESCALE_3,
    MSP432_PWM_PRESCALE_4,
    MSP432_PWM_PRESCALE_5,
    MSP432_PWM_PRESCALE_6,
    MSP432_PWM_PRESCALE_7,
    MSP432_PWM_PRESCALE_8,
} msp432_pwm_prescaler_t;

struct Msp432_PwmPin_prv
{
    msp432_pwm_t channel;
    msp432_pwm_pin_t pin;
};

typedef enum
{
    MSP432_PWM_MODE_OUT,
    MSP432_PWM_MODE_SET,
    MSP432_PWM_MODE_TOGGLE_RESET,
    MSP432_PWM_MODE_SET_RESET,
    MSP432_PWM_MODE_TOGGLE,
    MSP432_PWM_MODE_RESET,
    MSP432_PWM_MODE_TOGGLE_SET,
    MSP432_PWM_MODE_RESET_SET,
} msp432_pwm_mode_t;

COMPILE_ASSERT(MSP432_PWM_PRESCALE_8 == 0x7, pwm_check_psc_enum);

/**
 * Initialize own of the output pins on a pwm channel
 * @param pin pwm pin to initialize
 * @param mode mode operation mode of the pin
 */
void msp432_pwm_init_pin(PwmPin pin, msp432_pwm_mode_t mode);

/**
 * Find the best set of prescalers to use
 * for pwm TimerA
 * @param frequency (input) base frequency
 * @param psc_1 (output) psc_1
 * @param psc_2 (output) psc_2
 */
void msp432_pwm_optimal_prescaler(
        F64 frequency,
        msp432_pwm_prescaler_t* psc_1,
        msp432_pwm_prescaler_t* psc_2);

/**
 * Stop the pulse on a pwm channel
 * @param pwm channel to stop pulse on
 */
void msp432_pwm_channel_stop(msp432_pwm_t pwm);

/**
 * Start the timer to run pwm
 * @param pwm pwm channel
 */
void msp432_pwm_channel_start(msp432_pwm_t pwm);

#endif //CMPE460_MSP432_PWM_H
