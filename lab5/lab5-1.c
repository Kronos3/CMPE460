#include <switch.h>
#include <uartlib.h>
#include <led.h>
#include <tim.h>

// Override the weak reference default handler
void PORT1_IRQHandler(void)
{
    uprintf("Button pressed %d\r\n", switch_get_interrupt());

    // Clear all switch interrupts
    switch_clear_interrupt(switch_get_interrupt());

    static bool_t timer_enabled = FALSE;
    timer_enabled = !timer_enabled;
    tim32_set(TIM32_1, timer_enabled);
}

static void timer_task(tim32_t t)
{
    (void) t;
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

int main(void)
{
    uart_init(UART_USB, 9600);
    led_init(LED1_ALL);
    tim32_init(TIM32_1, timer_task,
               tim_calculate_arr(TIM32_PSC_1, 2.0), // trigger twice per second
               TIM32_PSC_1,
               TIM32_MODE_PERIODIC);
    switch_init(SWITCH_ALL, TRUE);

    while(1);
}
