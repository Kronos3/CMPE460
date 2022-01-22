#include <switch.h>
#include <uartlib.h>
#include <led.h>
#include <tim.h>

// Override the weak reference default handler
static U32 timer_counter_ms = 0;
static bool_t timer_running = FALSE;

static void switch_1_handler(void)
{
    static bool_t timer_enabled = FALSE;
    timer_enabled = !timer_enabled;
    tim32_reset(TIM32_1);
    tim32_set(TIM32_1, timer_enabled);
}

static void switch_2_handler(void)
{
    if (timer_running)
    {
        timer_running = FALSE;
        tim32_set(TIM32_2, FALSE);
        uprintf("Timer ran for %d ms\r\n", timer_counter_ms);
    }
    else
    {
        timer_running = TRUE;
        timer_counter_ms = 0;
        tim32_reset(TIM32_2);
        tim32_set(TIM32_2, TRUE);
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
    static U32 i = 0;
    const static U32 colors[] = {
            LED2_RED, // r
            LED2_GREEN, // g
            LED2_BLUE, // b
            LED2_GREEN | LED2_BLUE, // c
            LED2_RED | LED2_BLUE, // m
            LED2_RED | LED2_GREEN, // y
            LED2_RED | LED2_GREEN | LED2_BLUE, // w
    };

    if (timer_counter_ms % 500 == 0)
    {
        led2_set(colors[i++]);
        i %= sizeof(colors) / sizeof(colors[0]);
        (void) i;
    }
    timer_counter_ms++;
}

int main(void)
{
    uart_init(UART_USB, 9600);
    led_init(LED_ALL);
    tim32_init(TIM32_1, timer1_task,
               tim_calculate_arr(TIM32_PSC_1, 2.0), // trigger twice per second
               TIM32_PSC_1,
               TIM32_MODE_PERIODIC);
    tim32_init(TIM32_2, timer2_task,
               tim_calculate_arr(TIM32_PSC_1, 1000.0), // trigger 1ms
               TIM32_PSC_1,
               TIM32_MODE_PERIODIC);

    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_1_handler);
    switch_init(SWITCH_2, SWITCH_INT_PRESS, switch_2_handler);
}
