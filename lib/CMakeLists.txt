gen_library(gbl gbl.c gbl.h)

gen_library(oled oled.c oled.h)
target_link_libraries(oled i2c)

gen_library(uartlib uartlib.c uartlib.h)
target_link_libraries(uartlib uart)

gen_library(cam cam.c cam.h)
target_link_libraries(cam adc tim gbl gpio)

gen_library(dc dc.c dc.h)
target_link_libraries(dc pwm)

gen_library(i2c_mux i2c_mux.c i2c_mux.h)
target_link_libraries(i2c_mux i2c)

#add_subdirectory(ardu_cam)

gen_library(steering steering.c steering.h)
target_link_libraries(steering pwm)

gen_library(math math.c math.h)
target_link_libraries(math m)

gen_library(drive drive.c drive_math.c drive.h)
target_link_libraries(drive dc oled math steering cam gbl)
