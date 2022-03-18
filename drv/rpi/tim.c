#include <drv/rpi/arm_timer.h>
#include <bcm2835_int.h>
#include <drv/rpi/bcm2835.h>
#include <drv/tim.h>
#include <drv/rpi/tim.h>

typedef struct
{
    volatile U32 value;
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
        BCM2835_CORE_CLK_HZ / BCM2835_TIMER_BASE_FREQ < UINT32_MAX
        && "Base frequency not high enough to fit with prescaler of 1",
        bcm_phys_freq
);

static bool_t global_initialized = FALSE;

static void arm_timer_interrupt_irq(U32 irq)
{
    (void) irq;
    for (U32 i = 0; i < TIM_N; i++)
    {
        if (virtual_timer.timers[i].configured &&
            virtual_timer.timers[i].running)
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
    BCM2835_ARMTIMER->IRQ_CLR_ACK = 0x1;
}

void __attribute__((interrupt("FIQ")))
fast_interrupt_vector(void)
{
}

static void global_init(void)
{
    if (global_initialized)
    {
        return;
    }

    U32 reload = BCM2835_ARM_TIMER_FREQ / BCM2835_TIMER_BASE_FREQ;
    BCM2835_ARMTIMER->LOAD = reload;

    BCM2835_ARMTIMER->CTRL =
            ARM_TIMER_CTRL_INT_EN
            | ARM_TIMER_CTRL_32_BIT
            | ARM_TIMER_CTRL_PRESCALE_1;

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
    self->running = FALSE;
}

void tim_start(tim_t timer)
{
    FW_ASSERT(timer < TIM_N, timer);
    FW_ASSERT(virtual_timer.timers[timer].configured && "Timer not initialized",
              timer);

    virtual_timer.timers[timer].value = virtual_timer.timers[timer].reload;
    virtual_timer.timers[timer].running = TRUE;

    if (virtual_timer.running_n++ == 0)
    {
        // No timers are running yet meaning
        // the physical timer should be started

        // Reset the counter
        BCM2835_ARMTIMER->LOAD = BCM2835_ARMTIMER->RELOAD;

        // Enable the timer
        BCM2835_ARMTIMER->CTRL |= ARM_TIMER_CTRL_EN;
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
        BCM2835_ARMTIMER->CTRL &= ~ARM_TIMER_CTRL_EN;
    }
}
