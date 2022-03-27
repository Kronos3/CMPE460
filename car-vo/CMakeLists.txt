build_elf(car-vo i2c.c interrupt.c cpu.c main.c)
target_link_libraries(car-vo dc steering switch uart uartlib led)
