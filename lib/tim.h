#ifndef CMPE460_TIM_H
#define CMPE460_TIM_H

#include <fw.h>

#ifndef __tim_LINKED__
#error "You need to link 'tim' to use this header"
#endif

typedef enum
{
    TIM32_1,
    TIM32_2,
} tim32_t;

typedef enum
{
    TIM32_PSC_1 = 1,          //!< Max at 89 seconds
    TIM32_PSC_16 = 16,        //!< Max at 23 minutes
    TIM32_PSC_256 = 256,      //!< Max at 6 hours 21 minutes
} tim32_psc_t;

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
    TIM32_MODE_PERIODIC,

    // Counts down from the LOAD value and generates an interrupt when
    // it reaches 0, the timer then stops and waits for software to retrigger it
    TIM32_MODE_ONESHOT,
} tim32_mode_t;

/**
 * Initialize a timer32 with interrupts to run a certain task
 * @param timer timer to initialize
 * @param task task to run on interrupt
 * @param arr auto reload register (period)
 * @param psc prescaler (clock divider)
 */
void tim32_init(tim32_t timer,
                void (*task)(void),
                U32 arr,
                tim32_psc_t psc,
                tim32_mode_t mode);

/**
 * Helper function to calculate LOAD value for timer
 * given the prescaler we are planning to use
 * @param psc prescaler to use on timer
 * @param hz interrupt rate per second
 * @return calculated load/arr value for timer
 */
U32 tim_calculate_arr(U32 psc, F64 hz);

/**
 * Calculate the number of counts to achieve a certain
 * delay given the pre-scaled clock
 * @param prescaler prescaler on the clock you are polling
 * @param seconds seconds to wait
 * @return number of counts on the clock
 */
U32 tim_calculate_delay(U32 prescaler, F64 seconds);

/**
 * Start a timer
 * @param timer timer to start
 * @param enabled TRUE to enable, FALSE to disable
 */
void tim32_set(tim32_t timer, bool_t enabled);

/**
 * Get the current count of the timer
 * @param timer timer to read
 * @return current count on the timer
 */
U32 tim32_get(tim32_t timer);

/**
 * Reset the timer32 by reloading the LOAD register
 * If the timer is running it will restart, otherwise it will
 * simply reset and wait for an enable signal
 * @param timer timer to reset
 */
void tim32_reset(tim32_t timer);

/**
 * Initialize the systick timer to call a function
 * on every interrupt
 * @param arr timer period (no prescaler - use tim_calculate_arr(1, freq))
 */
void tim_systick_init(U32 arr);

/**
 * Enable or disable the SYSTICK timer
 * @param enabled TRUE to enable, FALSE to disable
 */
void tim_systick_set(bool_t enabled);

#endif //CMPE460_TIM_H
