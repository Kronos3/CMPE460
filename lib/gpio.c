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

void gpio_init(gpio_port_t port,
               U32 pin_mask,
               gpio_function_t function)
{
    switch (port)
    {
        case GPIO_PORT_1:
        case GPIO_PORT_3:
        case GPIO_PORT_5:
            gpio_init_Odd(port, pin_mask, function);
            break;
        case GPIO_PORT_2:
        case GPIO_PORT_4:
            FW_ASSERT(0 && "Even ports are not currently supported for GPIO", port);
        default:
            FW_ASSERT(0 && "Invalid port", port);
    }
}

void gpio_options(gpio_port_t port,
                  U32 pin_mask,
                  gpio_options_t options)
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
        case GPIO_PORT_2:
        case GPIO_PORT_4:
        default:
            FW_ASSERT(0 && "This port is not implemented yet", port);
    }

    if (options & GPIO_OPTIONS_HIGH_DRIVE_STRENGTH)
    {
        port_ptr->DS |= pin_mask;
    }

    FW_ASSERT(!(options & GPIO_OPTIONS_DIRECTION_OUTPUT &&
                options & GPIO_OPTIONS_DIRECTION_INPUT) && "Pin cannot be both input and output");

    if (options & GPIO_OPTIONS_DIRECTION_OUTPUT)
    {
        port_ptr->DIR |= pin_mask;
    }

    if (options & GPIO_OPTIONS_DIRECTION_INPUT)
    {
        port_ptr->DIR &= ~pin_mask;
    }
}

void gpio_output(gpio_port_t port,
                 U32 pin_mask,
                 bool_t output)
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
        case GPIO_PORT_2:
        case GPIO_PORT_4:
        default:
            FW_ASSERT(0 && "This port is not implemented yet", port);
    }

    if (output)
    {
        port_ptr->OUT |= pin_mask;
    }
    else
    {
        port_ptr->OUT &= ~pin_mask;
    }
}
