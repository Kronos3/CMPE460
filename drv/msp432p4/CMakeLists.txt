gen_library(gpio gpio.c)

gen_library(uart uart.c)

gen_library(led led.c)
target_link_libraries(led gpio)

gen_library(i2c i2c.c)
target_link_libraries(i2c gpio)

gen_library(switch switch.c)

gen_library(tim tim.c)

gen_library(adc adc.c)

gen_library(pwm pwm.c)
target_link_libraries(pwm gpio)
