gen_library(gpio gpio.c)
target_link_libraries(gpio)

gen_library(tim tim.c)
target_link_libraries(tim)

gen_library(i2c i2c.c)
target_link_libraries(i2c)

gen_library(dma dma.c)

gen_library(spi spi.c)
target_link_libraries(spi)
