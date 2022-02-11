# Bluetooth targets
build_elf(lab4-bt bt/main-bt.c)
target_link_libraries(lab4-bt uart)

build_elf(lab4-bt-led bt/lab4-bt-led.c)
target_link_libraries(lab4-bt-led uart led)

build_elf(lab4-bt-chat bt/lab4-bt-chat.c)
target_link_libraries(lab4-bt-chat uart)

# I2C targets
build_elf(lab4-i2c i2c/lab4-i2c.c)
target_link_libraries(lab4-i2c oled uart uartlib)
