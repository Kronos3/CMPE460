#ifndef CMPE460_LED_H
#define CMPE460_LED_H

typedef enum {
    LED1 = 1 << 0,
    LED1_ALL = 0x1,
    LED1_SHIFT = 0,

    LED2_RED = 1 << 16,
    LED2_GREEN = 1 << 17,
    LED2_BLUE = 1 << 18,
    LED2_ALL = 0x7,
    LED2_SHIFT = 16
} led_t;

/**
 * Initialize all LEDs on the MSP432 board
 */
void led_init(void);

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
