#include <msp432.h>
#include <gpio.h>


static void gpio_init_Odd(gpio_port_t port,
                          U32 pin_mask,
                          gpio_function_t function)
{
    DIO_PORT_Odd_Interruptable_Type* port_ptr;
    switch(port)
    {
        case GPIO_PORT_1:
            port_ptr = P1;
            break;
        case GPIO_PORT_3:
            port_ptr = P3;
            break;
        case GPIO_PORT_5:
            port_ptr = P5;
            break;
        default:
            FW_ASSERT(0 && "Invalid port", port);
    }

    switch(function)
    {
        case GPIO_FUNCTION_GENERAL: /* 00 */
            port_ptr->SEL1 &= ~pin_mask;
            port_ptr->SEL0 &= ~pin_mask;
            break;
        case GPIO_FUNCTION_PRIMARY: /* 01 */
            port_ptr->SEL1 &= ~pin_mask;
            port_ptr->SEL0 |= pin_mask;
            break;
        case GPIO_FUNCTION_SECONDARY: /* 10 */
            port_ptr->SEL1 |= pin_mask;
            port_ptr->SEL0 &= ~pin_mask;
            break;
        case GPIO_FUNCTION_TERTIARY: /* 11 */
            port_ptr->SEL1 |= pin_mask;
            port_ptr->SEL0 |= pin_mask;
            break;
        default:
            FW_ASSERT(0 && "Invalid function", function);
    }
}

void gpio_init(GpioPin pin,
               gpio_function_t function)
{
    switch (pin.port)
    {
        case GPIO_PORT_1:
        case GPIO_PORT_3:
        case GPIO_PORT_5:
            gpio_init_Odd(pin.port, pin.pin, function);
            break;
        case GPIO_PORT_2:
        case GPIO_PORT_4:
            FW_ASSERT(0 && "Even ports are not currently supported for GPIO", pin.port);
        default:
            FW_ASSERT(0 && "Invalid port", pin.port);
    }
}

void gpio_options(GpioPin pin,
                  gpio_options_t options)
{
    DIO_PORT_Odd_Interruptable_Type* port_ptr;
    switch(pin.port)
    {
        case GPIO_PORT_1:
            port_ptr = P1;
            break;
        case GPIO_PORT_3:
            port_ptr = P3;
            break;
        case GPIO_PORT_5:
            port_ptr = P5;
            break;
        case GPIO_PORT_2:
        case GPIO_PORT_4:
        default:
            FW_ASSERT(0 && "This port is not implemented yet", pin.port);
    }

    if (options & GPIO_OPTIONS_HIGH_DRIVE_STRENGTH)
    {
        port_ptr->DS |= pin.pin;
    }

    FW_ASSERT(!(options & GPIO_OPTIONS_DIRECTION_OUTPUT &&
                options & GPIO_OPTIONS_DIRECTION_INPUT) && "Pin cannot be both input and output");

    if (options & GPIO_OPTIONS_DIRECTION_OUTPUT)
    {
        port_ptr->DIR |= pin.pin;
    }

    if (options & GPIO_OPTIONS_DIRECTION_INPUT)
    {
        port_ptr->DIR &= ~pin.pin;
    }
}

void gpio_output(GpioPin pin,
                 bool_t output)
{
    DIO_PORT_Odd_Interruptable_Type* port_ptr;
    switch(pin.port)
    {
        case GPIO_PORT_1:
            port_ptr = P1;
            break;
        case GPIO_PORT_3:
            port_ptr = P3;
            break;
        case GPIO_PORT_5:
            port_ptr = P5;
            break;
        case GPIO_PORT_2:
        case GPIO_PORT_4:
        default:
            FW_ASSERT(0 && "This port is not implemented yet", pin.port);
    }

    if (output)
    {
        port_ptr->OUT |= pin.pin;
    }
    else
    {
        port_ptr->OUT &= ~pin.pin;
    }
}
