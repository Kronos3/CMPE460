#include <drv/gpio.h>

static void __attribute__((naked))
dummy(U32 ra)
{
    __asm__("bx lr");
}

//-------------------------------------------------------------------------
int main(void)
{
    U32 ra;
    gpio_init(GPIO_ACT_LED, GPIO_FUNCTION_OUTPUT);

    // Flash the LED
    while (1)
    {
        gpio_output(GPIO_ACT_LED, TRUE);
        for (ra = 0; ra < 0x100000; ra++) dummy(ra);
        gpio_output(GPIO_ACT_LED, FALSE);
        for (ra = 0; ra < 0x100000; ra++) dummy(ra);
    }
}
