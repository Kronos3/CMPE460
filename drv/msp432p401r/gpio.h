#ifndef CMPE460_MSP432_GPIO_H
#define CMPE460_MSP432_GPIO_H

typedef struct GpioPin_prv GpioPin;

typedef enum
{
    GPIO_PORT_1,
    GPIO_PORT_2,
    GPIO_PORT_3,
    GPIO_PORT_4,
    GPIO_PORT_5,
    GPIO_PORT_6,
    GPIO_PORT_7,
    GPIO_PORT_8,
    GPIO_PORT_9,
    GPIO_PORT_10,
} gpio_port_t;

#define GPIO_PIN(port_, pin_) {GPIO_PORT_##port_, (1 << (pin_))}

typedef enum
{
    GPIO_FUNCTION_GENERAL,
    GPIO_FUNCTION_PRIMARY,
    GPIO_FUNCTION_SECONDARY,
    GPIO_FUNCTION_TERTIARY,
} gpio_function_t;

typedef enum
{
    GPIO_OPTIONS_HIGH_DRIVE_STRENGTH = 1 << 0,
    GPIO_OPTIONS_DIRECTION_OUTPUT = 1 << 1,
    GPIO_OPTIONS_DIRECTION_INPUT = 1 << 2,
} gpio_options_t;

struct GpioPin_prv
{
    gpio_port_t port;
    U32 pin;
};

#endif //CMPE460_MSP432_GPIO_H
