#include <lib/dc.h>
#include <drv/uart.h>
#include <drv/tim.h>

static enum {
    M_0_100_FORWARD,
    M_100_0_FORWARD,
    M_0_100_REVERSE,
    M_100_0_REVERSE,
    M_N
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

int main(void)
{
    uart_init(UART_USB, 115200);

    PwmPin forward = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_1
    };
    PwmPin backward = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_2
    };
    dc_cfg(DC_0, forward, backward, 10000);
    dc_init();

    // 10ms delay between ticks
    tim_init(TIM32_1, tick_handler, 100.0);

    // Start the pwm and timers
    dc_start();
    tim_start(TIM32_1);

    while (TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}

