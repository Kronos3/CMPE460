build_elf(lab6-pwm lab6-pwm.c)
target_link_libraries(lab6-pwm switch uartlib tim pwm)

# Use 48Mhz clock
target_compile_definitions(lab6-pwm PUBLIC __SYSTEM_CLOCK=48000000)
