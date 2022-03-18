#include <drv/tim.h>
#include <drv/gpio.h>
#include <instr.h>

void tim_handler()
{
    static I32 i = 0;
    gpio_output(GPIO_ACT_LED, i++ % 2);
}

void main(void)
{
    gpio_init(GPIO_ACT_LED, GPIO_FUNCTION_OUTPUT);

    // Run a 2 Hz clock signal
    tim_init(TIM_0, tim_handler, 4);
    tim_start(TIM_0);

    ENABLE_INTERRUPTS();
}
