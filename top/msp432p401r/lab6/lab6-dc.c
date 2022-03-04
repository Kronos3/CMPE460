#include <lib/dc.h>
#include <drv/uart.h>
#include <drv/tim.h>
#include <drv/msp432p401r/switch.h>

static enum {
    M_0_100_FORWARD,
    M_100_0_FORWARD,
    M_0_100_REVERSE,
    M_100_0_REVERSE,
    M_N = 2
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
//        case M_0_100_REVERSE:
//            dc_set(DC_0, -state_i / 100.0);
//            break;
//        case M_100_0_REVERSE:
//            dc_set(DC_0, (100 - state_i) / -100.0);
//            break;
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
            break;
        default:
        case TRUE:
            tim_start(TIM32_1);
            break;
    }
}

int main(void)
{
    uart_init(UART_USB, 115200);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, toggle_timer);

    // P2.4
    PwmPin forward = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_3
    };
    // P2.5
    PwmPin backward = {
            MSP432_PWM_0,
            MSP432_PWM_PIN_4
    };
    dc_cfg(DC_0, forward, backward, 10000);
//    dc_init();

    // 10ms delay between ticks
    tim_init(TIM32_1, tick_handler, 10.0);

    // Start the pwm and timers
//    dc_start();
//    dc_set(DC_0, -1);
    pwm_init(forward, 10000);
    pwm_init(backward, 10000);
//    pwm_init(backward, 10000);
    pwm_set(forward, 0.8);
    pwm_set(backward, 0.2);
//    GpioPin backward_pin = GPIO_PIN(2,4);
//    gpio_init(backward_pin, GPIO_FUNCTION_GENERAL);
//    gpio_options(backward_pin, GPIO_OPTIONS_DIRECTION_OUTPUT | GPIO_OPTIONS_HIGH_DRIVE_STRENGTH);
//    gpio_output(backward_pin, FALSE);
    pwm_start(forward);
    pwm_start(backward);
//    pwm_start(backward);

    while (TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}

