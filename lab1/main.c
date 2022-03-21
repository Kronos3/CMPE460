/*
Title:  Lab1 Demo
Purpose:   Blink an LED on button press
Name: Andrei Tumbar
Date: 01/14/2022

This file prepared by LJBeato
01/11/2021

*/
#include "msp.h"
#include <fw.h>

// Number of loop cycles before the light changes color
#define LOOP_DELAY_N (1 << 14)

// LED1 is on pin 1.0
#define LED1 (P1)
#define LED1_PIN (0)

// LED2 has three different LEDs
#define LED2 (P2)
#define LED2_RED (0)
#define LED2_GREEN (1)
#define LED2_BLUE (2)

// User buttons
#define USR_BTN (P1)
#define USR_BTN1 (1)
#define USR_BTN2 (4)

#define LED_ON(port, pin) (port)->OUT |= BIT(pin)
#define LED_OFF(port, pin) (port)->OUT &= ~BIT(pin)

// Main Program
// 1. Init LEDs
// 2. Init Switch
// 3. init state=0
// 4. If SW1 pressed, light LED1
// 5. if SW2 pressed, cycle next color triplet of RGB,CMY,W
//	First press - RGB
//  Second press - CMY
//  Third press - W
//  Repeat
// 6. If button released
//		Turn off the LED
//
//
void LED1_Init(void)
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

void LED2_Init(void)
{
    // configure PortPin for LED2 as port I/O
    U32 all_pins = BIT(LED2_RED) | BIT(LED2_GREEN) | BIT(LED2_BLUE);
    LED2->SEL0 &= ~all_pins;
    LED2->SEL1 &= ~all_pins;

    // make built-in LED2 LEDs high drive strength
    LED2->DS |= all_pins;

    // make built-in LED2 out
    LED2->DIR |= all_pins;

    // turn off LED
    LED2->OUT &= ~(all_pins);
}

void Switches_Init(void)
{
    U32 all_pins = BIT(USR_BTN1) | BIT(USR_BTN2);
    // configure PortPin for Switch 1 and Switch2 as port I/O
    USR_BTN->SEL0 &= ~all_pins;
    USR_BTN->SEL1 &= ~all_pins;

    // configure as input
    USR_BTN->DIR &= ~all_pins;

    // pull down resistor feature
    USR_BTN->REN |= all_pins;
//    USR_BTN->OUT &= ~all_pins;
    USR_BTN->OUT |= all_pins;
}

//------------Switch_Input------------
// Read and return the status of Switch1
// Input: none
// return: TRUE if pressed
//         FALSE if not pressed
bool_t Switch1_Pressed(void)
{
    // return TRUE(pressed) or FALSE(not pressed)
    return !!!(USR_BTN->IN & BIT(USR_BTN1));
}

//------------Switch_Input------------
// Read and return the status of Switch2
// Input: none
// return: TRUE if pressed
//         FALSE if not pressed
bool_t Switch2_Pressed(void)
{
    // return TRUE(pressed) or FALSE(not pressed)
    return !!!(USR_BTN->IN & BIT(USR_BTN2));
}

int main(void)
{
    //Initialize any supporting variables
    U32 loop_delay_i = 0;
    bool_t led2_active = 0;
    U32 led2_color_i = 0;
    U32 led2_color_j = 0;
    U32 led2_colors[][3] = {
            {
                    BIT(LED2_RED),
                    BIT(LED2_GREEN),
                    BIT(LED2_BLUE),
            },
            {
                    BIT(LED2_GREEN) | BIT(LED2_BLUE), // cyan
                    BIT(LED2_RED) | BIT(LED2_BLUE), // magenta
                    BIT(LED2_GREEN) | BIT(LED2_RED), // yellow
            },
            {
                    // all white
                    BIT(LED2_RED) | BIT(LED2_GREEN) | BIT(LED2_BLUE),
                    BIT(LED2_RED) | BIT(LED2_GREEN) | BIT(LED2_BLUE),
                    BIT(LED2_RED) | BIT(LED2_GREEN) | BIT(LED2_BLUE),
            }
    };

    // Initialize buttons and LEDs
    LED1_Init();
    LED2_Init();
    Switches_Init();

    for (;;)
    {
        //Turn on an LED1 (RED) when button 1 is held
        if (Switch1_Pressed())
        {
            // turn on LED1
            LED_ON(LED1, LED1_PIN);
        }
        else
        {
            // turn off LED1
            LED_OFF(LED1, LED1_PIN);
        }

        // now do the same for LED2
        if (Switch2_Pressed())
        {
            if (!led2_active)
            {
                led2_active = TRUE;
                led2_color_j = 0;
                LED2->OUT = led2_colors[led2_color_i][led2_color_j];
            }
        }
        else
        {
            if (led2_active && loop_delay_i++ == LOOP_DELAY_N)
            {
                loop_delay_i = 0;
                led2_color_j++;
                if (led2_color_j == 3)
                {
                    // Wait for the next button press
                    // Go to the next color cycle
                    // Turn off all LEDs
                    led2_color_i = (led2_color_i + 1) % 3;
                    led2_active = FALSE;
                    LED2->OUT = 0;
                }
                else
                {
                    LED2->OUT = led2_colors[led2_color_i][led2_color_j];
                }
            }
        }
    }
}

