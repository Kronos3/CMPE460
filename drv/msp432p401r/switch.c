#include <drv/msp432p401r/switch.h>
#include <msp432.h>
#include <nvic.h>

#define USR_BTN (P1)

static switch_t interrupts_initialized = 0;
static void (*tasks[2])(void) = {NULL, NULL};

void switch_init(switch_t switches,
                 switch_int_t switch_interrupt,
                 void (*task)(void))
{
    // configure PortPin for Switch 1 and Switch2 as port I/O
    USR_BTN->SEL0 &= ~switches;
    USR_BTN->SEL1 &= ~switches;

    // configure as input
    USR_BTN->DIR &= ~switches;

    // pull down resistor feature
    USR_BTN->REN |= switches;
    USR_BTN->OUT |= switches;

    if (switch_interrupt != SWITCH_INT_NONE)
    {
        DISABLE_INTERRUPTS();

        interrupts_initialized |= switches;

        if (switches & SWITCH_1)
        {
            tasks[0] = task;
        }

        if (switches & SWITCH_2)
        {
            tasks[1] = task;
        }

        //7-0 PxIFG RW 0h Port X interrupt flag
        //0b = No interrupt is pending.
        //1b = Interrupt is pending.
        // clear flag1 (reduce possibility of extra interrupt)
        P1->IFG &= ~switches;

        //7-0 PxIE RW 0h Port X interrupt enable
        //0b = Corresponding port interrupt disabled
        //1b = Corresponding port interrupt enabled
        // ARM interrupt on P1.1
        P1->IE |= switches;

        //7-0 PxIES RW Undefined Port X interrupt edge select
        //0b = PxIFG flag is set with a low-to-high transition.
        //1b = PxIFG flag is set with a high-to-low transition
        // now set the pin to cause falling edge interrupt event
        // P1.1 is falling edge event
        switch(switch_interrupt)
        {
            case SWITCH_INT_PRESS:
                P1->IES |= switches;
                break;
            case SWITCH_INT_RELEASE:
                P1->IES &= ~switches;
                break;
            default:
                FW_ASSERT(0 && "Invalid switch interrupt mode", switch_interrupt);
        }

        // now set the pin to cause falling edge interrupt event
        NVIC_IPR8 = (NVIC_IPR8 & 0x00FFFFFF) | 0x40000000; // priority 2

        // enable Port 1 - interrupt 35 in NVIC
        NVIC_ISER1 = 0x00000008;

        ENABLE_INTERRUPTS();
    }
}

bool_t switch_get(switch_t id)
{
    // return TRUE(pressed) or FALSE(not pressed)
    return (USR_BTN->IN & id) ? TRUE : FALSE;
}

void switch_clear_interrupt(switch_t switches)
{
    P1->IFG &= ~switches;
}

switch_t switch_get_interrupt(void)
{
    return P1->IFG & interrupts_initialized;
}

void PORT1_IRQHandler(void)
{
    U32 who = switch_get_interrupt();
    if ((who & SWITCH_1) && tasks[0])
    {
        tasks[0]();
    }

    if ((who & SWITCH_2) && tasks[1])
    {
        tasks[1]();
    }

    // Clear all switch interrupts
    switch_clear_interrupt(who);
}
