#ifndef CMPE460_LED_H
#define CMPE460_LED_H

#include <fw.h>

#ifndef __led_LINKED__
#error "You need to link 'led' to use this header"
#endif

typedef enum {
    LED1_SHIFT = 0,
    LED1 = 1 << (LED1_SHIFT + 0),
    LED1_ALL = LED1,

    LED2_SHIFT = 16,
    LED2_RED = 1 << (LED2_SHIFT + 0),
    LED2_GREEN = 1 << (LED2_SHIFT + 1),
    LED2_BLUE = 1 << (LED2_SHIFT + 2),
    LED2_ALL = LED2_RED | LED2_GREEN | LED2_BLUE,

    LED_ALL = LED1_ALL | LED2_ALL,
} led_t;

COMPILE_ASSERT(LED1_ALL >> LED1_SHIFT == 0x1, led1_all_value);
COMPILE_ASSERT(LED2_ALL >> LED2_SHIFT == 0x7, led2_all_value);

/**
 * Initialize all LEDs on the MSP432 board
 * @param led Mask of which LEDs to initialize
 */
void led_init(led_t led);

/**
 * Turn on a set of leds
 * @param mask leds to turn on
 */
void led_on(led_t mask);

/**
 * Turn on or OFF leds respecting all bits in the mask
 * @param mask LED mask
 */
void led_set(led_t mask);

/**
 * Turn on a set of leds
 * @param mask leds to turn on off
 */
void led_off(led_t mask);

#endif //CMPE460_LED_H
