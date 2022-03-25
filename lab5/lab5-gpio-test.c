#include <drv/adc.h>
#include <lib/uartlib.h>
#include <drv/tim.h>
#include <drv/msp432p4/switch.h>
#include <drv/gpio.h>

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
    switch(timer_state)
    {
        case FALSE:
            tim_start(TIM32_1);
            break;
        case TRUE:
            tim_stop(TIM32_1);
            break;
        default:
            FW_ASSERT(0, timer_state);
    }
}

int main(void)
{
    uart_init(UART_USB, 9600);
    tim_init(TIM32_1, task, 100000);
    gpio_init(clk, GPIO_FUNCTION_GENERAL);
    gpio_options(clk, GPIO_OPTIONS_DIRECTION_OUTPUT | GPIO_OPTIONS_HIGH_DRIVE_STRENGTH);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);
}

