#include <msp.h>
#include "led.h"
#include <fw.h>

// LED1 is on pin 1.0
#define LED1 (P1)
#define LED1_PIN (0)

// LED2 has three different LEDs
#define LED2 (P2)
#define LED2_RED_PIN (0)
#define LED2_GREEN_PIN (1)
#define LED2_BLUE_PIN (2)

static void LED1_Init(void)
{
    // configure PortPin for LED1 as port I/O
    // set P1.0 as output
    LED1->SEL0 &= ~BIT(LED1_PIN);
    LED1->SEL1 &= ~BIT(LED1_PIN);

    // make built-in LED1 LED high drive strength
    LED1->DS |= BIT(LED1_PIN);

    // make built-in LED1 out
    LED1->DIR |= BIT(LED1_PIN);

    // turn off LED
    LED1->OUT &= ~BIT(LED1_PIN);
}

static void LED2_Init(void)
{
    // configure PortPin for LED2 as port I/O
    U32 all_pins = BIT(LED2_RED_PIN) | BIT(LED2_GREEN_PIN) | BIT(LED2_BLUE_PIN);
    LED2->SEL0 &= ~all_pins;
    LED2->SEL1 &= ~all_pins;

    // make built-in LED2 LEDs high drive strength
    LED2->DS |= all_pins;

    // make built-in LED2 out
    LED2->DIR |= all_pins;

    // turn off LED
    LED2->OUT &= ~(all_pins);
}

void led_init(void)
{
    LED1_Init();
    LED2_Init();
}

void led_on(led_t mask)
{
    LED1->OUT |= (mask >> LED1_SHIFT) & LED1_ALL;
    LED2->OUT |= (mask >> LED2_SHIFT) & LED2_ALL;
}

void led_set(led_t mask)
{
    // Set only the bits on the port to the required values
    LED1->OUT = (LED1->OUT & ~LED1_ALL) | ((mask >> LED1_SHIFT) & LED1_ALL);
    LED2->OUT = (LED2->OUT & ~LED2_ALL) | ((mask >> LED2_SHIFT) & LED2_ALL);
}

void led_off(led_t mask)
{
    LED1->OUT &= ~((mask >> LED1_SHIFT) & LED1_ALL);
    LED2->OUT &= ~((mask >> LED2_SHIFT) & LED2_ALL);
}
