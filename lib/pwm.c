#include <msp432.h>
#include <pwm.h>

typedef struct PwmRegistration_prv
{
    pwm_t id;
    GpioPin pin;
    Timer_A_Type* hw;
} PwmRegistration;

static const PwmRegistration pwm_registration[] = {
        {PWM_0, GPIO_PIN(3, 3), TIMER_A0},
        {PWM_1, GPIO_PIN(3, 3), TIMER_A1},
        {PWM_2, GPIO_PIN(3, 3), TIMER_A2},
        {PWM_3, GPIO_PIN(3, 3), TIMER_A3},
};

GpioPin pwm_init(pwm_t pwm, pwm_prescaler_t prescaler, F32 rate)
{

}

void pwm_set(pwm_t pwm, F32 duty)
{

}

F32 pwm_get(pwm_t pwm)
{

}

void pwm_start(pwm_t pwm)
{

}

void pwm_stop(pwm_t pwm)
{

}
