build_elf(os_test main.c)
target_link_libraries(os_test uartlib tim led tumbar_rtos)
