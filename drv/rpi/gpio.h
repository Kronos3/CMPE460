#ifndef CMPE460_RPI_ZERO_GPIO_H
#define CMPE460_RPI_ZERO_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GPIO_P1_03 = 2,  ///< Version 2, Pin P1-03
    GPIO_P1_05 = 3,  ///< Version 2, Pin P1-05
    GPIO_P1_07 = 4,  ///< Version 2, Pin P1-07
    GPIO_P1_08 = 14,  ///< Version 2, Pin P1-08, defaults to alt function 0 UART0_TXD
    GPIO_P1_10 = 15,  ///< Version 2, Pin P1-10, defaults to alt function 0 UART0_RXD
    GPIO_P1_11 = 17,  ///< Version 2, Pin P1-11
    GPIO_P1_12 = 18,  ///< Version 2, Pin P1-12, can be PWM channel 0 in ALT FUN 5
    GPIO_P1_13 = 27,  ///< Version 2, Pin P1-13
    GPIO_P1_15 = 22,  ///< Version 2, Pin P1-15
    GPIO_P1_16 = 23,  ///< Version 2, Pin P1-16
    GPIO_P1_18 = 24,  ///< Version 2, Pin P1-18
    GPIO_P1_19 = 10,  ///< Version 2, Pin P1-19, MOSI when SPI0 in use
    GPIO_P1_21 = 9,  ///< Version 2, Pin P1-21, MISO when SPI0 in use
    GPIO_P1_22 = 25,  ///< Version 2, Pin P1-22
    GPIO_P1_23 = 11,  ///< Version 2, Pin P1-23, CLK when SPI0 in use
    GPIO_P1_24 = 8,  ///< Version 2, Pin P1-24, CE0 when SPI0 in use
    GPIO_P1_26 = 7,  ///< Version 2, Pin P1-26, CE1 when SPI0 in use

    GPIO_ACT_LED = 47,  //!< On board LED (green)

    // RPi Version 2, new plug P5
    GPIO_P5_03 = 28,  ///< Version 2, Pin P5-03
    GPIO_P5_04 = 29,  ///< Version 2, Pin P5-04
    GPIO_P5_05 = 30,  ///< Version 2, Pin P5-05
    GPIO_P5_06 = 31,  ///< Version 2, Pin P5-06
} GpioPin;

typedef enum
{
    GPIO_FUNCTION_INPUT,
    GPIO_FUNCTION_OUTPUT,
    GPIO_FUNCTION_ALT0,
    GPIO_FUNCTION_ALT1,
    GPIO_FUNCTION_ALT2,
    GPIO_FUNCTION_ALT3,
    GPIO_FUNCTION_ALT4,
    GPIO_FUNCTION_ALT5,
} gpio_function_t;

typedef enum
{
    GPIO_OPTIONS_PUD_OFF,
    GPIO_OPTIONS_PUD_DOWN,
    GPIO_OPTIONS_PUD_UP,
} gpio_options_t;

#ifdef __cplusplus
}
#endif

#endif //CMPE460_RPI_ZERO_GPIO_H
