#ifndef CMPE460_TIM_H
#define CMPE460_TIM_H

#ifndef __tim_LINKED__
#error "You need to link 'tim' to use this header"
#endif

#ifdef __RPI_ZERO__
#include "rpi_zero/tim.h"
#elif defined(__MSP432P401R__)
#include "msp432p401r/tim.h"
#endif

/**
 * Initialize a timer to run a task at a certain rate
 * @param timer timer id to initialize
 * @param task
 * @param hz
 */
void tim_init(tim_t timer, void (*task)(void), F64 hz);

/**
 * Start a timer
 * @param timer timer to start
 */
void tim_start(tim_t timer);

/**
 * Stop a timer
 * @param timer timer to stop
 */
void tim_stop(tim_t timer);

#endif //CMPE460_TIM_H
