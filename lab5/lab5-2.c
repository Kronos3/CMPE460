#include <adc.h>
#include <uartlib.h>
#include <tim.h>
#include <switch.h>

static void task(void)
{
    uprintf("ADC reads: %u\r\n", adc_in());
}

static void switch_handler(void)
{
    // Stop the timer to stop triggering the ADC
    tim32_set(TIM32_1, FALSE);
}

int main(void)
{
    uart_init(UART_USB, 9600);
    adc_init();
    tim32_init(TIM32_1, task,
               tim_calculate_arr(TIM32_PSC_1, 2),
               TIM32_PSC_1, TIM32_MODE_PERIODIC);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);

    // Start the timer
    tim32_set(TIM32_1, TRUE);
}
