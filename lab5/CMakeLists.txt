build_elf(lab5-1 lab5-1.c)
target_link_libraries(lab5-1 switch uartlib led tim)

# Use 48Mhz clock
target_compile_definitions(lab5-1 PUBLIC __SYSTEM_CLOCK=48000000)

build_elf(lab5-2 lab5-2.c)
target_link_libraries(lab5-2 uartlib tim switch adc)
target_compile_definitions(lab5-2 PUBLIC __SYSTEM_CLOCK=48000000)
