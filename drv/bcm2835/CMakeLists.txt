gen_library(bcm2835 bcm2835.h bcm2835.c)

gen_library(gpio gpio.c)
target_link_libraries(gpio bcm2835)

gen_library(tim tim.c)
target_link_libraries(tim bcm2835)
