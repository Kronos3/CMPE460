#include <drv/tim.h>
#include <drv/gpio.h>
#include <instr.h>

static bool_t pin_level = FALSE;

static void tim_handler(void)
{
    gpio_output(GPIO_ACT_LED, FALSE);
}

void main(void)
{
    gpio_init(GPIO_ACT_LED, GPIO_FUNCTION_OUTPUT);
    gpio_output(GPIO_ACT_LED, FALSE);

    // Run a 2 Hz clock signal
    tim_init(TIM_0, tim_handler, 4);
    tim_start(TIM_0);

    ENABLE_INTERRUPTS();
}
