#include "tim.h"
#include <msp.h>
#include <arm.h>
#include <nvic.h>

static struct Timer32 {
    TimTask task;
    Timer32_Type* const timer;
    U32 arr;
    const U32 interrupt_nvic;
} timer32_table[TIM32_N] = {
        {NULL, TIMER32_1, 0, 1 << 25},
        {NULL, TIMER32_2, 0, 1 << 26},
};

void tim32_init(tim32_t timer,
                TimTask task,
                U32 arr,
                tim32_psc_t psc,
                tim32_mode_t mode)
{
    // Make sure a TIM32 was selected
    FW_ASSERT(timer < TIM32_N, timer);
    struct Timer32* t = &timer32_table[timer];

    DISABLE_INTERRUPTS();

    t->task = task;
    t->timer->LOAD = arr;
    t->arr = arr;

    // Set the common parameters
    // All modes are considered period modes
    // All modes will enable interrupts
    // All modes will use 32-bit timer counter
    t->timer->CONTROL =
            TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE |
            TIMER32_CONTROL_SIZE;

    // Select timer mode
    switch(mode)
    {
        case TIM32_MODE_PERIODIC:
            t->timer->CONTROL &= ~TIMER32_CONTROL_ONESHOT;
            break;
        case TIM32_MODE_ONESHOT:
            t->timer->CONTROL |= TIMER32_CONTROL_ONESHOT;
            break;
        default:
            FW_ASSERT(0 && "Invalid timer mode", mode);
    }

    // Select prescaler
    switch(psc)
    {
        case TIM32_PSC_1:
            t->timer->CONTROL =
                    (t->timer->CONTROL & ~TIMER32_CONTROL_PRESCALE_MASK)
                    | TIMER32_CONTROL_PRESCALE_0;
            break;
        case TIM32_PSC_16:
            t->timer->CONTROL =
                    (t->timer->CONTROL & ~TIMER32_CONTROL_PRESCALE_MASK)
                    | TIMER32_CONTROL_PRESCALE_1;
            break;
        case TIM32_PSC_256:
            t->timer->CONTROL =
                    (t->timer->CONTROL & ~TIMER32_CONTROL_PRESCALE_MASK)
                    | TIMER32_CONTROL_PRESCALE_2;
            break;
        default:
            FW_ASSERT(0 && "Invalid prescaler", psc);
    }

    // interrupts enabled in the main program after all devices initialized
    // NVIC_IPR6
    NVIC_IPR6 = (NVIC_IPR6 & 0xFFFF00FF) | 0x00004000; // priority 2

    // enable interrupt 25 in NVIC, NVIC_ISER0
    // NVIC_ISER0
    NVIC_ISER0 |= t->interrupt_nvic;

    ENABLE_INTERRUPTS();
}

static inline void
t32_int_common(tim32_t t)
{
    // Acknowledge Timer32 Timer interrupt
    timer32_table[t].timer->INTCLR = 1;

    // Execute user task
    FW_ASSERT(timer32_table[t].task && "No task provided", t);
    timer32_table[t].task(t);
}

void T32_INT1_IRQHandler(void)
{
    t32_int_common(TIM32_1);
}

void T32_INT2_IRQHandler(void)
{
    t32_int_common(TIM32_2);
}

U32 tim_calculate_arr(tim32_psc_t prescaler, F64 hz)
{
    return (U32) (((F64)SystemCoreClock / (F64)prescaler) / hz);
}

void tim32_set(tim32_t timer, bool_t enabled)
{
    // Make sure a TIM32 was selected
    FW_ASSERT(timer < TIM32_N, timer);
    struct Timer32* t = &timer32_table[timer];

    if (enabled) t->timer->CONTROL |= TIMER32_CONTROL_ENABLE;
    else t->timer->CONTROL &= ~TIMER32_CONTROL_ENABLE;
}

void tim32_reset(tim32_t timer)
{
    // Make sure a TIM32 was selected
    FW_ASSERT(timer < TIM32_N, timer);
    struct Timer32* t = &timer32_table[timer];
    t->timer->LOAD = t->arr;
}
