#ifndef CMPE460_SWITCH_H
#define CMPE460_SWITCH_H

#include <fw.h>

#ifndef __switch_LINKED__
#error "You need to link 'switch' to use this header"
#endif

// These correspond to the pins on port 1
typedef enum
{
    SWITCH_1 = 1 << 1,           // User button 1
    SWITCH_2 = 1 << 4,           // User button 2
    SWITCH_ALL = SWITCH_1 | SWITCH_2
} switch_t;

/**
 * Initialize both switches on the MSP432 board
 * @param switches Mask of switches you want to initialize
 * @param enable_interrupts Enable interrupts when buttons are pressed
 *                          (override the default handler for the interrupt)
 *                          PORT1_IRQHandler weak reference and can be overridden
 */
void switch_init(switch_t switches, bool_t enable_interrupts);

/**
 * Clear the interrupt signal of the button press
 * @param switches switches to clear signal for
 */
void switch_clear_interrupt(switch_t switches);

/**
 * Check which switches are interrupted
 * @return mask of all interrupted switches
 */
switch_t switch_get_interrupt(void);

/**
 * Get the status of the user button
 * @param id switch to get status of
 * @return TRUE if pressed, FALSE otherwise
 */
bool_t switch_get(switch_t id);

#endif //CMPE460_SWITCH_H
