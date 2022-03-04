#include <msp432.h>
#include <drv/pwm.h>

#define PWM_PIN_OPTIONS (GPIO_OPTIONS_DIRECTION_OUTPUT | GPIO_OPTIONS_HIGH_DRIVE_STRENGTH)

typedef struct PwmRegistration_prv
{
    Timer_A_Type* hw;
    struct
    {
        GpioPin pin;
        gpio_function_t function;
    } pins[5];
} PwmRegistration;

static const PwmRegistration pwm_registration[] = {
    {
        TIMER_A0,
        {
            {GPIO_PIN(7, 3), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(2, 4), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(2, 5), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(2, 6), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(2, 7), GPIO_FUNCTION_PRIMARY},
        },
    },
    {
        TIMER_A1,
        {
            {GPIO_PIN(8, 0), GPIO_FUNCTION_SECONDARY},
            {GPIO_PIN(7, 7), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(7, 6), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(7, 5), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(7, 4), GPIO_FUNCTION_PRIMARY},
        },
    },
    {
        TIMER_A2,
        {
            {GPIO_PIN(8, 1), GPIO_FUNCTION_SECONDARY},
            {GPIO_PIN(5, 6), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(5, 7), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(6, 6), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(6, 7), GPIO_FUNCTION_PRIMARY},
        },
    },
    {
        TIMER_A3,
        {
            {GPIO_PIN(10, 4), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(10, 5), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(8, 2), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(9, 2), GPIO_FUNCTION_PRIMARY},
            {GPIO_PIN(9, 3), GPIO_FUNCTION_PRIMARY},
        },
    },
};

void pwm_init(PwmPin pwm, F64 rate)
{
    msp432_pwm_prescaler_t prescaler_1;
    msp432_pwm_prescaler_t prescaler_2;

    msp432_pwm_optimal_prescaler(rate, &prescaler_1, &prescaler_2);

    // Validate the input parameters
    switch(pwm.channel)
    {
        case MSP432_PWM_0:
        case MSP432_PWM_1:
        case MSP432_PWM_2:
        case MSP432_PWM_3:
            break;
        default:
            FW_ASSERT(0, pwm);
    }

    U16 id_mask;
    switch(prescaler_1)
    {
        case MSP432_PWM_PRESCALE_1:
            id_mask = TIMER_A_CTL_ID__1;
            break;
        case MSP432_PWM_PRESCALE_2:
            id_mask = TIMER_A_CTL_ID__2;
            break;
        case MSP432_PWM_PRESCALE_4:
            id_mask = TIMER_A_CTL_ID__4;
            break;
        case MSP432_PWM_PRESCALE_8:
            id_mask = TIMER_A_CTL_ID__8;
            break;
        case MSP432_PWM_PRESCALE_3:
        case MSP432_PWM_PRESCALE_5:
        case MSP432_PWM_PRESCALE_6:
        case MSP432_PWM_PRESCALE_7:
        default:
            FW_ASSERT(0 && "Invalid psc_1, must one of 1,2,4,8", prescaler_1);
    }

    switch(prescaler_2)
    {
        case MSP432_PWM_PRESCALE_1:
        case MSP432_PWM_PRESCALE_2:
        case MSP432_PWM_PRESCALE_3:
        case MSP432_PWM_PRESCALE_4:
        case MSP432_PWM_PRESCALE_5:
        case MSP432_PWM_PRESCALE_6:
        case MSP432_PWM_PRESCALE_7:
        case MSP432_PWM_PRESCALE_8:
            break;
        default:
            FW_ASSERT(0 && "Invalid psc_2, must one of 1,2,3,4,5,6,7,8", prescaler_2);
    }

    const PwmRegistration* self = &pwm_registration[pwm.channel];
    U32 total_prescaler = (prescaler_1 + 1) * (prescaler_2 + 1);

    F64 prescaled_clock = (F64)SystemCoreClock / total_prescaler;
    F64 count_f = prescaled_clock / rate;
    U16 count = (U16)count_f;

    FW_ASSERT(count_f < UINT16_MAX && "Need a higher prescaler",
              (prescaler_1 + 1), (prescaler_2 + 1), count, UINT16_MAX);

    self->hw->CTL =
            TIMER_A_CTL_SSEL__SMCLK
            | id_mask
            | TIMER_A_CTL_MC__STOP // turn on later
            ;

    self->hw->EX0 = prescaler_2;

    // Set total period
    self->hw->CCR[0] = count;

    // Load the prescaler and the settings
    // Don't start the clock yet
    self->hw->CTL |= TIMER_A_CTL_CLR;

    msp432_pwm_init_pin(pwm, MSP432_PWM_MODE_RESET_SET);
}

void msp432_pwm_init_pin(PwmPin pin, msp432_pwm_mode_t mode)
{
    FW_ASSERT(pin.channel >= MSP432_PWM_0 && pin.channel <= MSP432_PWM_3, pin.channel);
    FW_ASSERT(pin.pin >= MSP432_PWM_PIN_0 && pin.pin < MSP432_PWM_PIN_N);

    const PwmRegistration* self = &pwm_registration[pin.channel];

    // Place in compare + no interrupt
    self->hw->CCTL[pin.pin] &= ~(TIMER_A_CCTLN_CAP | TIMER_A_CCTLN_CCIE);

    // Clear out-mode
    self->hw->CCTL[pin.pin] &= ~TIMER_A_CCTLN_OUTMOD_MASK;

    // Set out mode
    switch (mode)
    {
        case MSP432_PWM_MODE_OUT:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_0;
            break;
        case MSP432_PWM_MODE_SET:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_1;
            break;
        case MSP432_PWM_MODE_TOGGLE_RESET:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_2;
            break;
        case MSP432_PWM_MODE_SET_RESET:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_3;
            break;
        case MSP432_PWM_MODE_TOGGLE:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_4;
            break;
        case MSP432_PWM_MODE_RESET:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_5;
            break;
        case MSP432_PWM_MODE_TOGGLE_SET:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_6;
            break;
        case MSP432_PWM_MODE_RESET_SET:
            self->hw->CCTL[pin.pin] |= TIMER_A_CCTLN_OUTMOD_7;
            break;
        default:
            FW_ASSERT(0 && "Invalid PWM mode", mode);
    }

    // Initialize gpio pins
    gpio_init(self->pins[pin.pin].pin, self->pins[pin.pin].function);
    gpio_options(self->pins[pin.pin].pin, PWM_PIN_OPTIONS);
}

void pwm_set(PwmPin pin, F64 duty)
{
    FW_ASSERT(pin.channel >= MSP432_PWM_0 && pin.channel <= MSP432_PWM_3, pin.channel);
    FW_ASSERT(pin.pin >= MSP432_PWM_PIN_0 && pin.pin < MSP432_PWM_PIN_N);
    FW_ASSERT(duty >= 0.0 && duty <= 1.0, (U32)(100 * duty));

    const PwmRegistration* self = &pwm_registration[pin.channel];
    self->hw->CCR[pin.pin] = (U16)(self->hw->CCR[0] * duty);
}

void pwm_start(PwmPin pwm)
{
    FW_ASSERT(pwm.channel >= MSP432_PWM_0 && pwm.channel <= MSP432_PWM_3, pwm);
    const PwmRegistration* self = &pwm_registration[pwm.channel];
    self->hw->CTL |= TIMER_A_CTL_MC__UP;
}

void pwm_stop(PwmPin pwm)
{
    FW_ASSERT(pwm.channel >= MSP432_PWM_0 && pwm.channel <= MSP432_PWM_3, pwm);
    const PwmRegistration* self = &pwm_registration[pwm.channel];

    // Clear all running flags
    self->hw->CTL &= ~TIMER_A_CTL_MC_MASK;
}

void msp432_pwm_optimal_prescaler(
        F64 frequency,
        msp432_pwm_prescaler_t* psc_1,
        msp432_pwm_prescaler_t* psc_2)
{
    // psc_1 must be a power of 2
    // psc_2 can be anything between 1 and 8
    // Important note: The encoding for psc is actually (psc - 1)
    //                 So a divider of 1 will be encoded as 0
    for (I32 psc_1_iter = 1; psc_1_iter <= 8; psc_1_iter *= 2)
    {
        for (I32 psc_2_iter = 1; psc_2_iter <= 8; psc_2_iter++)
        {
            F64 prescaled_clock = (F64)SystemCoreClock / (psc_1_iter * psc_2_iter);
            F64 count_f = prescaled_clock / frequency;
            U16 count = (U16)count_f;

            if (count < UINT16_MAX)
            {
                // We found our prescalers
                *psc_1 = (psc_1_iter - 1);
                *psc_2 = (psc_2_iter - 1);
                return;
            }
        }
    }

    FW_ASSERT(0 && "Failed to resolve prescalers");
}
