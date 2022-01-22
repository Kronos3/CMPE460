#include <adc.h>
#include <uartlib.h>
#include <tim.h>
#include <switch.h>

static void task(void)
{
    uprintf("ADC reads: %f V\r\n", adc_voltage(adc_in()));
}

static void switch_handler(void)
{
    // Toggle the timer to start/stop triggering the ADC
    static bool_t timer_state = FALSE;
    timer_state = !timer_state;
    tim32_set(TIM32_1, timer_state);
}

int main(void)
{
    uart_init(UART_USB, 9600);
    adc_init();
    tim32_init(TIM32_1, task,
               tim_calculate_arr(TIM32_PSC_1, 2),
               TIM32_PSC_1, TIM32_MODE_PERIODIC);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);
}
