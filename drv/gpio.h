#ifndef CMPE460_GPIO_H
#define CMPE460_GPIO_H

#include <fw.h>

#ifndef __gpio_LINKED__
#error "You need to link 'gpio' to use this header"
#endif

#ifdef __BCM2835__
#include "bcm2835/gpio.h"
#elif defined(__MSP432P401R__)
#include "msp432p401r/gpio.h"
#endif

/**
 * Set up a GPIO pin (or set of pins) on a specific port
 * @param pin pin or set of pins to initialize
 * @param function function to initialize pins to
 */
void gpio_init(GpioPin pin, gpio_function_t function);

/**
 * Set up more options on a set of pins
 * @param pin pin or set of pins to set options for
 * @param options options to set
 */
void gpio_options(GpioPin pin, gpio_options_t options);

/**
 * Set a GPIO pin to a value
 * @param pin pin to set GPIO value
 * @param output value to set pins to
 */
void gpio_output(GpioPin pin, bool_t output);

#endif //CMPE460_GPIO_H
