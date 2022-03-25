#include <lib/dc.h>
#include <drv/uart.h>
#include <drv/tim.h>
#include <drv/msp432p4/switch.h>
#include <instr.h>

static enum {
    M_0_100_FORWARD,
    M_100_0_FORWARD,
    M_0_100_REVERSE,
    M_100_0_REVERSE,
    M_N = 4
} state = M_0_100_FORWARD;
static I32 state_i = 0;

static void tick_handler(void)
{
    switch(state)
    {
        case M_0_100_FORWARD:
            dc_set(DC_0, state_i / 100.0);
            break;
        case M_100_0_FORWARD:
            dc_set(DC_0, (100 - state_i) / 100.0);
            break;
        case M_0_100_REVERSE:
            dc_set(DC_0, -state_i / 100.0);
            break;
        case M_100_0_REVERSE:
            dc_set(DC_0, (100 - state_i) / -100.0);
            break;
        default:
        case M_N:
            FW_ASSERT(0, state);
    }

    if (++state_i == 100)
    {
        state = (state + 1) % M_N;
        state_i = 0;
    }
}

static bool_t timer_running = FALSE;
static void toggle_timer(void)
{
    timer_running = !timer_running;
    switch (timer_running)
    {
        case FALSE:
            tim_stop(TIM32_1);
            dc_set(DC_0, 0.0);
            break;
        default:
        case TRUE:
            state_i = 0;
            state = M_0_100_FORWARD;
            tim_start(TIM32_1);
            break;
    }
}

int main(void)
{
    uart_init(UART_USB, 115200);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, toggle_timer);

    DcParam motor = {
            // P2.4
            .forward = {
                MSP432_PWM_0,
                        MSP432_PWM_PIN_1
            },
            // P2.6
            .backward = {
                    MSP432_PWM_0,
                    MSP432_PWM_PIN_3
            },
            .base_frequency = 10000
    };
    dc_cfg(DC_0, &motor);
    dc_init();

    // 10ms delay between ticks
    tim_init(TIM32_1, tick_handler, 100.0);

    // Start the pwm and timers
    dc_start();

    while(1)
    {
        WAIT_FOR_INTERRUPT();
    }
}

