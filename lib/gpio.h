#ifndef CMPE460_GPIO_H
#define CMPE460_GPIO_H

#include <fw.h>

#ifndef __gpio_LINKED__
#error "You need to link 'gpio' to use this header"
#endif

typedef enum
{
    GPIO_PORT_1,
    GPIO_PORT_2,
    GPIO_PORT_3,
    GPIO_PORT_4,
    GPIO_PORT_5,
} gpio_port_t;

typedef enum
{
    GPIO_FUNCTION_GENERAL,
    GPIO_FUNCTION_PRIMARY,
    GPIO_FUNCTION_SECONDARY,
    GPIO_FUNCTION_TERTIARY,
} gpio_function_t;

typedef enum
{
    GPIO_OPTIONS_HIGH_DRIVE_STRENGTH = 1 << 0,
    GPIO_OPTIONS_DIRECTION_OUTPUT = 1 << 1,
    GPIO_OPTIONS_DIRECTION_INPUT = 1 << 2,
} gpio_options_t;

/**
 * Set up a GPIO pin (or set of pins) on a specific port
 * @param port port where pins reside
 * @param pin_mask set of pins to initialize
 * @param function function to initialize pins to
 */
void gpio_init(gpio_port_t port,
               U32 pin_mask,
               gpio_function_t function);

/**
 * Set up more options on a set of pins
 * @param port port where pins reside
 * @param pin_mask set of pins to set options for
 * @param options options to set
 */
void gpio_options(gpio_port_t port,
                  U32 pin_mask,
                  gpio_options_t options);

/**
 * Set a GPIO pin to a value
 * @param port port of pin
 * @param pin_mask pins to set
 * @param output value to set pins to
 */
void gpio_output(gpio_port_t port, U32 pin_mask, bool_t output);

#endif //CMPE460_GPIO_H
