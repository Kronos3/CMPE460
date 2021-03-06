#include <lib/uartlib.h>
#include <drv/tim.h>

#include <drv/msp432p4/switch.h>
#include <drv/msp432p4/led.h>

// Override the weak reference default handler
static U32 timer_counter_ms = 0;
static bool_t timer_running = FALSE;
static U32 led_2_i = 0;

const static U32 led_2_colors[] = {
        LED2_RED, // r
        LED2_GREEN, // g
        LED2_BLUE, // b
        LED2_GREEN | LED2_BLUE, // c
        LED2_RED | LED2_BLUE, // m
        LED2_RED | LED2_GREEN, // y
        LED2_RED | LED2_GREEN | LED2_BLUE, // w
};

static void switch_1_handler(void)
{
    static bool_t timer_enabled = FALSE;
    timer_enabled = !timer_enabled;
    msp432_tim32_reset(MSP432_TIM32_1);
    msp432_tim32_set(MSP432_TIM32_1, timer_enabled);

    if (!timer_enabled)
    {
        led_off(LED1);
    }
}

static void switch_2_handler(void)
{
    if (timer_running)
    {
        timer_running = FALSE;
        msp432_tim32_set(MSP432_TIM32_2, FALSE);
        led_off(LED2_ALL);
        uprintf("Timer ran for %d ms\r\n", timer_counter_ms);
        led_2_i = (led_2_i + 1) % (sizeof(led_2_colors) / sizeof(led_2_colors[0]));
    }
    else
    {
        timer_running = TRUE;
        timer_counter_ms = 0;
        msp432_tim32_reset(MSP432_TIM32_2);
        msp432_tim32_set(MSP432_TIM32_2, TRUE);
        led2_set(led_2_colors[led_2_i]);
    }
}

static void timer1_task(void)
{
    static U32 led_status = 0;
    led_status++;
    if (led_status % 2 == 0)
    {
        led_off(LED1);
    }
    else
    {
        led_on(LED1);
    }
}

static void timer2_task(void)
{
    timer_counter_ms++;
}

int main(void)
{
    uart_init(UART_USB, 9600);
    led_init(LED_ALL);
    tim_init(TIM32_1, timer1_task, 2.0);
    tim_init(TIM32_2, timer2_task, 1000.0);

    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_1_handler);
    switch_init(SWITCH_2, SWITCH_INT_PRESS, switch_2_handler);
}
