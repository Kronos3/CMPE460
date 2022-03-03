#include <msp432.h>
#include <drv/gpio.h>


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
        case GPIO_PORT_7:
            port_ptr = P7;
            break;
        case GPIO_PORT_9:
            port_ptr = P9;
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

static void gpio_init_Even(gpio_port_t port,
                           U32 pin_mask,
                           gpio_function_t function)
{
    DIO_PORT_Even_Interruptable_Type* port_ptr;
    switch(port)
    {
        case GPIO_PORT_2:
            port_ptr = P2;
            break;
        case GPIO_PORT_4:
            port_ptr = P4;
            break;
        case GPIO_PORT_6:
            port_ptr = P6;
            break;
        case GPIO_PORT_8:
            port_ptr = P8;
            break;
        case GPIO_PORT_10:
            port_ptr = P10;
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
        case GPIO_PORT_7:
        case GPIO_PORT_9:
            gpio_init_Odd(pin.port, pin.pin, function);
            break;
        case GPIO_PORT_2:
        case GPIO_PORT_4:
        case GPIO_PORT_6:
        case GPIO_PORT_8:
        case GPIO_PORT_10:
            gpio_init_Even(pin.port, pin.pin, function);
            break;
        default:
            FW_ASSERT(0 && "Invalid port", pin.port);
    }
}

void gpio_options(GpioPin pin,
                  gpio_options_t options)
{
    DIO_PORT_Odd_Interruptable_Type* odd_ptr = NULL;
    DIO_PORT_Even_Interruptable_Type* even_ptr = NULL;
    switch(pin.port)
    {
        case GPIO_PORT_1:
            odd_ptr = P1;
            break;
        case GPIO_PORT_3:
            odd_ptr = P3;
            break;
        case GPIO_PORT_5:
            odd_ptr = P5;
            break;
        case GPIO_PORT_7:
            odd_ptr = P7;
            break;
        case GPIO_PORT_9:
            odd_ptr = P9;
            break;
        case GPIO_PORT_2:
            even_ptr = P2;
            break;
        case GPIO_PORT_4:
            even_ptr = P4;
            break;
        case GPIO_PORT_6:
            even_ptr = P6;
            break;
        case GPIO_PORT_8:
            even_ptr = P8;
            break;
        case GPIO_PORT_10:
            even_ptr = P10;
            break;
        default:
            FW_ASSERT(0 && "This port is not implemented yet", pin.port);
    }

    if (odd_ptr)
    {
        if (options & GPIO_OPTIONS_HIGH_DRIVE_STRENGTH)
        {
            odd_ptr->DS |= pin.pin;
        }

        FW_ASSERT(!(options & GPIO_OPTIONS_DIRECTION_OUTPUT &&
                    options & GPIO_OPTIONS_DIRECTION_INPUT) && "Pin cannot be both input and output");

        if (options & GPIO_OPTIONS_DIRECTION_OUTPUT)
        {
            odd_ptr->DIR |= pin.pin;
        }

        if (options & GPIO_OPTIONS_DIRECTION_INPUT)
        {
            odd_ptr->DIR &= ~pin.pin;
        }
    }
    else if (even_ptr)
    {
        if (options & GPIO_OPTIONS_HIGH_DRIVE_STRENGTH)
        {
            even_ptr->DS |= pin.pin;
        }

        FW_ASSERT(!(options & GPIO_OPTIONS_DIRECTION_OUTPUT &&
                    options & GPIO_OPTIONS_DIRECTION_INPUT) && "Pin cannot be both input and output");

        if (options & GPIO_OPTIONS_DIRECTION_OUTPUT)
        {
            even_ptr->DIR |= pin.pin;
        }

        if (options & GPIO_OPTIONS_DIRECTION_INPUT)
        {
            even_ptr->DIR &= ~pin.pin;
        }
    }
    else
    {
        FW_ASSERT(0);
    }
}

void gpio_output(GpioPin pin,
                 bool_t output)
{
    DIO_PORT_Odd_Interruptable_Type* odd_ptr = NULL;
    DIO_PORT_Even_Interruptable_Type* even_ptr = NULL;
    switch(pin.port)
    {
        case GPIO_PORT_1:
            odd_ptr = P1;
            break;
        case GPIO_PORT_3:
            odd_ptr = P3;
            break;
        case GPIO_PORT_5:
            odd_ptr = P5;
            break;
        case GPIO_PORT_7:
            odd_ptr = P7;
            break;
        case GPIO_PORT_9:
            odd_ptr = P9;
            break;
        case GPIO_PORT_2:
            even_ptr = P2;
            break;
        case GPIO_PORT_4:
            even_ptr = P4;
            break;
        case GPIO_PORT_6:
            even_ptr = P6;
            break;
        case GPIO_PORT_8:
            even_ptr = P8;
            break;
        case GPIO_PORT_10:
            even_ptr = P10;
            break;
        default:
            FW_ASSERT(0 && "This port is not implemented yet", pin.port);
    }

    if (odd_ptr)
    {
        if (output)
        {
            odd_ptr->OUT |= pin.pin;
        }
        else
        {
            odd_ptr->OUT &= ~pin.pin;
        }
    }
    else if (even_ptr)
    {
        if (output)
        {
            even_ptr->OUT |= pin.pin;
        }
        else
        {
            even_ptr->OUT &= ~pin.pin;
        }
    }
    else
    {
        FW_ASSERT(0);
    }
}
