#include <drv/tim.h>
#include <drv/gpio.h>

#define LED_PIN GPIO_P1_16

static bool_t pin_level = FALSE;
static void tim_handler(void)
{
    pin_level = !pin_level;
    gpio_output(LED_PIN, pin_level);
}

void main(void)
{
    gpio_init(LED_PIN, GPIO_FUNCTION_OUTPUT);

    // Run a 2 Hz clock signal
    tim_init(TIM_0, tim_handler, 4);
    tim_start(TIM_0);
}
