gen_library(gbl gbl.c gbl.h)

gen_library(oled oled.c oled.h)
target_link_libraries(oled i2c)

gen_library(uartlib uartlib.c uartlib.h)
target_link_libraries(uartlib uart)

gen_library(cam cam.c cam.h)
target_link_libraries(cam adc tim gbl gpio)

gen_library(dc dc.c dc.h)
target_link_libraries(dc pwm)
