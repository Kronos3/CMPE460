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
    TIM32_N,
} tim32_t;

typedef void (*TimTask)(tim32_t timer);

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
                TimTask task,
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
U32 tim_calculate_arr(tim32_psc_t psc, F64 hz);

/**
 * Start a timer
 * @param timer timer to start
 * @param enabled TRUE to enable, FALSE to disable
 */
void tim32_set(tim32_t timer, bool_t enabled);

#endif //CMPE460_TIM_H
