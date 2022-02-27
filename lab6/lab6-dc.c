#include <uart.h>
#include <pwm.h>
#include <tim.h>

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
            pwm_set_pin(PWM_0, PWM_PIN_1, state_i / 100.0);
            pwm_set_pin(PWM_0, PWM_PIN_2, 0.0);
            break;
        case M_100_0_FORWARD:
            pwm_set_pin(PWM_0, PWM_PIN_1, (100 - state_i) / 100.0);
            pwm_set_pin(PWM_0, PWM_PIN_2, 0.0);
            break;
        case M_0_100_REVERSE:
            pwm_set_pin(PWM_0, PWM_PIN_2, state_i / 100.0);
            pwm_set_pin(PWM_0, PWM_PIN_1, 0.0);
            break;
        case M_100_0_REVERSE:
            pwm_set_pin(PWM_0, PWM_PIN_2, (100 - state_i) / 100.0);
            pwm_set_pin(PWM_0, PWM_PIN_1, 0.0);
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
    pwm_init(PWM_0, PWM_PRESCALE_1, PWM_PRESCALE_4, 10000);

    pwm_init_pin(PWM_0, PWM_MODE_RESET_SET, PWM_PIN_1);
    pwm_init_pin(PWM_0, PWM_MODE_RESET_SET, PWM_PIN_2);

    // 10ms delay between ticks
    tim32_init(TIM32_1, tick_handler, tim_calculate_arr(1, 100), 1, TIM32_MODE_PERIODIC);

    // Start the pwm and timers
    pwm_start(PWM_0);
    tim32_set(TIM32_1, TRUE);

    while (TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}

