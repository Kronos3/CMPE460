#ifndef CMPE460_ARM_TIMER_H
#define CMPE460_ARM_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fw.h>
#include <drv/bcm2835/bcm2835.h>

/**
 * The ARM Timer is based on a ARM AP804, but it has a number of differences with the standard
 *  SP804:
 *  - There is only one timer.
 *  - It only runs in continuous mode.
 *  - It has a extra clock pre-divider register.
 *  - It has a extra stop-in-debug-mode control bit.
 *  - It also has a 32-bit free running counter.
 *  The clock from the ARM timer is derived from the system clock. This clock can change dynamically
 *  e.g. if the system goes into reduced power or in low power mode. Thus the clock speed adapts to
 *  the overall system performance capabilities. For accurate timing it is recommended to use the
 *  system timers
 */

typedef struct
{
    volatile U32 LOAD;
    volatile U32 VALUE;         // read only
    volatile U32 CTRL;
    volatile U32 IRQ_CLR_ACK;   // write only
    volatile U32 RAW_IRQ;       // read only
    volatile U32 MSK_IRQ;       // read only
    volatile U32 RELOAD;
    volatile U32 PRE_DIVIDER;
    volatile U32 FREE_RUNNING;
} ArmTimer;

enum
{
    ARM_TIMER_CTRL_32_BIT = 1 << 1,
    ARM_TIMER_CTRL_PRESCALE_1 = 0 << 2,
    ARM_TIMER_CTRL_PRESCALE_16 = 1 << 2,
    ARM_TIMER_CTRL_PRESCALE_256 = 2 << 2,
    ARM_TIMER_CTRL_INT_EN = 1 << 5,
    ARM_TIMER_CTRL_EN = 1 << 7,
};

/** @brief See the documentation for the ARM side timer (Section 14 of the
    BCM2835 Peripherals PDF) */
#define BCM2835_ARMTIMER_BASE               ( BCM2835_PERI_BASE + 0xB400UL )
#define BCM2835_ARMTIMER                    ( (ArmTimer*)BCM2835_ARMTIMER_BASE )
#define BCM2835_ARM_TIMER_FREQ              ( 1000000 )

#ifdef __cplusplus
}
#endif

#endif //CMPE460_ARM_TIMER_H
