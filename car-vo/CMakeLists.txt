build_elf(car-vo-i2c common.c i2c.c interrupt.c cpu.c main-i2c.c)
target_link_libraries(car-vo-i2c uartlib dc steering switch uart uartlib led)
#target_compile_options(car-vo-i2c PRIVATE -O0)
target_compile_definitions(car-vo-i2c PUBLIC __SYSTEM_CLOCK=48000000)

build_elf(car-vo-uart common.c main-uart.c)
target_link_libraries(car-vo-uart uartlib dc steering switch uart led)
target_compile_definitions(car-vo-uart PUBLIC __SYSTEM_CLOCK=48000000)
