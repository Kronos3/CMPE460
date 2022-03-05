#include <bcm2835_int.h>
#include <drv/bcm2835/bcm2835.h>
#include <drv/tim.h>
#include <drv/bcm2835/tim.h>
#include <fw.h>

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

#define ARM_TIMER_BASE (0x7E00B000)
#define ARM_TIMER_BASE_W_OFS (ARM_TIMER_BASE + 0x400)

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
    ARM_TIMER_CTRL_EN = 1 << 7,
    ARM_TIMER_CTRL_INT_EN = 1 << 7,
    ARM_TIMER_CTRL_23_BIT = 1 << 7
};

#define ARM_TIMER ((ArmTimer*) ARM_TIMER_BASE_W_OFS)

typedef struct
{
    U32 value;
    U32 reload;
    void (*task)(void);
    bool_t running;
    bool_t configured;
} VirtualTimer;

static struct
{
    U32 running_n;
    VirtualTimer timers[TIM_N];
} virtual_timer = {0};

COMPILE_ASSERT(
        BCM2835_CORE_CLK_HZ / BCM2835_TIMER_BASE_FREQ < UINT16_MAX
        && "Base frequency not high enough to fit with prescaler of 1",
        bcm_phys_freq
);

static bool_t global_initialized = FALSE;

static void arm_timer_interrupt_irq(U32 irq)
{
    (void) irq;
    for (U32 i = 0; i < TIM_N; i++)
    {
        if (virtual_timer.timers[i].running)
        {
            if (!--virtual_timer.timers[i].value)
            {
                if (virtual_timer.timers[i].task)
                {
                    virtual_timer.timers[i].task();
                }

                virtual_timer.timers[i].value = virtual_timer.timers[i].reload;
            }
        }
    }

    // Clear the interrupt
    ARM_TIMER->IRQ_CLR_ACK = 0x1;
}

static void global_init(void)
{
    if (global_initialized)
    {
        return;
    }

    ARM_TIMER->CTRL = ARM_TIMER_CTRL_INT_EN;
    ARM_TIMER->RELOAD = BCM2835_CORE_CLK_HZ / BCM2835_TIMER_BASE_FREQ;

    global_initialized = TRUE;

    // Set up the interrupt in the IRQ table
    bcm2835_interrupt_register(BCM2835_IRQ_ID_TIMER_0, arm_timer_interrupt_irq);
    bcm2835_interrupt_enable(BCM2835_IRQ_ID_TIMER_0);
}

void tim_init(tim_t timer, void (*task)(void), F64 hz)
{
    global_init();

    FW_ASSERT(timer < TIM_N, timer);
    FW_ASSERT(task);

    FW_ASSERT(hz < BCM2835_TIMER_BASE_FREQ && "Requested frequency too high"
                                              "try raising BCM2835_TIMER_BASE_FREQ");

    VirtualTimer* self = &virtual_timer.timers[timer];
    self->reload = BCM2835_TIMER_BASE_FREQ / hz;
    self->task = task;
    self->configured = TRUE;
}

void tim_start(tim_t timer)
{
    FW_ASSERT(timer < TIM_N, timer);
    FW_ASSERT(virtual_timer.timers[timer].configured && "Timer not initialized",
              timer);

    virtual_timer.timers[timer].value = 1;
    virtual_timer.timers[timer].running = TRUE;

    if (virtual_timer.running_n++ == 0)
    {
        // No timers are running yet meaning
        // the physical timer should be started

        // Reset the counter
        ARM_TIMER->LOAD = ARM_TIMER->RELOAD;

        // Enable the timer
        ARM_TIMER->CTRL |= ARM_TIMER_CTRL_EN;
    }
}

void tim_stop(tim_t timer)
{
    FW_ASSERT(timer < TIM_N, timer);
    FW_ASSERT(virtual_timer.timers[timer].configured && "Timer not initialized",
              timer);

    virtual_timer.timers[timer].running = FALSE;

    if (--virtual_timer.running_n == 0)
    {
        // All virtual timers have stopped
        // We can stop the physical timer
        ARM_TIMER->CTRL &= ~ARM_TIMER_CTRL_EN;
    }
}
