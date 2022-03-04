#include <drv/gpio.h>
#include <drv/bcm2835/bcm2835.h>

void gpio_init(GpioPin pin, gpio_function_t function)
{
    switch (function)
    {
        case GPIO_FUNCTION_INPUT:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
            break;
        case GPIO_FUNCTION_OUTPUT:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
            break;
        case GPIO_FUNCTION_ALT0:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT0);
            break;
        case GPIO_FUNCTION_ALT1:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT1);
            break;
        case GPIO_FUNCTION_ALT2:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT2);
            break;
        case GPIO_FUNCTION_ALT3:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT3);
            break;
        case GPIO_FUNCTION_ALT4:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT4);
            break;
        case GPIO_FUNCTION_ALT5:
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT5);
            break;
        default:
            FW_ASSERT(0 && "Invalid function select", function);
    }
}

void gpio_options(GpioPin pin, gpio_options_t options)
{
    switch(options)
    {
        case GPIO_OPTIONS_PUD_OFF:
            bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_OFF);
            break;
        case GPIO_OPTIONS_PUD_DOWN:
            bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_DOWN);
            break;
        case GPIO_OPTIONS_PUD_UP:
            bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);
            break;
        default:
            FW_ASSERT(0 && "Invalid options select", options);
    }
}

void gpio_output(GpioPin pin, bool_t output)
{
    switch(output)
    {
        case FALSE:
            bcm2835_gpio_clr(pin);
            break;
        case TRUE:
        default:
            bcm2835_gpio_set(pin);
            break;
    }
}
