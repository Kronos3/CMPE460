#include <adc.h>
#include <uartlib.h>
#include <tim.h>
#include <switch.h>
#include <gpio.h>

static const GpioPin clk = {GPIO_PORT_5, 1 << 4};
//static const GpioPin si = {GPIO_PORT_5, 1 << 5};

static void task(void)
{
    static bool_t value = FALSE;
    value = !value;
    gpio_output(clk, value);
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
    tim32_init(TIM32_1, task,
               tim_calculate_arr(TIM32_PSC_1, 100000),
               TIM32_PSC_1, TIM32_MODE_PERIODIC);
    gpio_init(clk, GPIO_FUNCTION_GENERAL);
    gpio_options(clk, GPIO_OPTIONS_DIRECTION_OUTPUT | GPIO_OPTIONS_HIGH_DRIVE_STRENGTH);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);
}

