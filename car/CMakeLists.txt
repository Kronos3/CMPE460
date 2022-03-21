build_elf(car i2c.c interrupt.c cpu.c main.c)
target_link_libraries(car pwm uart uartlib led)
