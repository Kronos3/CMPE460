#include <msp.h>
#include <drv/msp432p4/led.h>

// LED1 is on pin 1.0
#define LED1 (P1)

// LED2 has three different LEDs
#define LED2 (P2)

// Extract the pin bits for a certain port given the front end ID
#define PIN_SEL(mask, p) (((mask) & LED##p##_ALL) >> LED##p##_SHIFT)

#define LED_INIT_M(P, PINS) \
do {                           \
    /* General purpose pin mode */ \
    (P)->SEL0 &= ~(PINS); \
    (P)->SEL1 &= ~(PINS); \
\
    /* make built-in LEDs high drive strength */ \
    (P)->DS |= (PINS); \
\
    /* make built-in pin output */ \
    (P)->DIR |= (PINS); \
 \
    /* turn off LED */ \
    (P)->OUT &= ~(PINS); \
} while(0)

void led_init(led_t led)
{
    // Initialize all selected LEDs on their respective ports
    LED_INIT_M(LED1, PIN_SEL(led, 1));
    LED_INIT_M(LED2, PIN_SEL(led, 2));
}

void led_on(led_t mask)
{
    LED1->OUT |= PIN_SEL(mask, 1);
    LED2->OUT |= PIN_SEL(mask, 2);
}

void led1_set(led_t mask)
{
    // Set only the bits on the port to the required values
    LED1->OUT = (LED1->OUT & ~(LED1_ALL >> LED1_SHIFT)) | PIN_SEL(mask, 1);
}

void led2_set(led_t mask)
{
    // Set only the bits on the port to the required values
    LED2->OUT = (LED2->OUT & ~(LED2_ALL >> LED2_SHIFT)) | PIN_SEL(mask, 2);
}

void led_off(led_t mask)
{
    LED1->OUT &= ~PIN_SEL(mask, 1);
    LED2->OUT &= ~PIN_SEL(mask, 2);
}
