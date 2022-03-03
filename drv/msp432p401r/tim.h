#ifndef CMPE460_MSP432_TIM_H
#define CMPE460_MSP432_TIM_H

#include <fw.h>

// For external use
typedef enum
{
    TIM32_1,
    TIM32_2,
    SYSTICK,
} tim_t;

// For internal use
typedef enum
{
    MSP432_TIM32_1,
    MSP432_TIM32_2,
} msp432_tim32_t;

typedef enum
{
    MSP432_TIM32_PSC_1 = 1,          //!< Max at 89 seconds
    MSP432_TIM32_PSC_16 = 16,        //!< Max at 23 minutes
    MSP432_TIM32_PSC_256 = 256,      //!< Max at 6 hours 21 minutes
} msp432_tim32_psc_t;

// Simplified modes (consolidates a couple of options)
// All of these modes will do the following:
//      1. Generate interrupts handled by task
//      2. 32-bit timer counter
//      3. Start disabled
typedef enum
{
    // Counts down from the LOAD value and generates an interrupt when
    // it reaches 0, the LOAD value is automatically reloaded and the
    // timer continues
    MSP432_TIM32_MODE_PERIODIC,

    // Counts down from the LOAD value and generates an interrupt when
    // it reaches 0, the timer then stops and waits for software to re-trigger it
    MSP432_TIM32_MODE_ONESHOT,
} msp432_tim32_mode_t;

/**
 * Initialize a timer32 with interrupts to run a certain task
 * @param timer timer to initialize
 * @param task task to run on interrupt
 * @param arr auto reload register (period)
 * @param psc prescaler (clock divider)
 */
void msp432_tim32_init(msp432_tim32_t timer,
                       void (*task)(void),
                       U32 arr,
                       msp432_tim32_psc_t psc,
                       msp432_tim32_mode_t mode);

/**
 * Helper function to calculate LOAD value for timer
 * given the prescaler we are planning to use
 * @param psc prescaler to use on timer
 * @param hz interrupt rate per second
 * @return calculated load/arr value for timer
 */
U32 msp432_tim_calculate_arr(msp432_tim32_psc_t psc, F64 hz);
msp432_tim32_psc_t msp432_tim32_calculate_prescaler(F64 hz);

/**
 * Start a timer
 * @param timer timer to start
 * @param enabled TRUE to enable, FALSE to disable
 */
void msp432_tim32_set(msp432_tim32_t timer, bool_t enabled);

/**
 * Get the current count of the timer
 * @param timer timer to read
 * @return current count on the timer
 */
U32 msp432_tim32_get(msp432_tim32_t timer);

/**
 * Reset the timer32 by reloading the LOAD register
 * If the timer is running it will restart, otherwise it will
 * simply reset and wait for an enable signal
 * @param timer timer to reset
 */
void msp432_tim32_reset(msp432_tim32_t timer);

/**
 * Initialize the systick timer to call a function
 * on every interrupt
 * @param task task to run on systick interrupt
 * @param arr timer period (no prescaler - use tim_calculate_arr(1, freq))
 */
void msp432_tim_systick_init(void (*task)(void), U32 arr);

/**
 * Enable or disable the SYSTICK timer
 * @param enabled TRUE to enable, FALSE to disable
 */
void msp432_tim_systick_set(bool_t enabled);

#endif //CMPE460_MSP432_TIM_H
