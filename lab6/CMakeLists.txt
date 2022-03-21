build_elf(lab6-pwm lab6-pwm.c)
target_link_libraries(lab6-pwm switch uartlib pwm)

# Use 48Mhz clock
target_compile_definitions(lab6-pwm PUBLIC __SYSTEM_CLOCK=48000000)

build_elf(lab6-dc lab6-dc.c)
target_link_libraries(lab6-dc dc tim switch uartlib)

# Use 48Mhz clock
target_compile_definitions(lab6-dc PUBLIC __SYSTEM_CLOCK=48000000)

build_elf(lab6-servo lab6-servo.c)
target_link_libraries(lab6-servo dc tim switch uartlib)

# Use 48Mhz clock
target_compile_definitions(lab6-servo PUBLIC __SYSTEM_CLOCK=48000000)

build_elf(lab6-simul lab6-simul.c)
target_link_libraries(lab6-simul dc tim switch uartlib)

# Use 48Mhz clock
target_compile_definitions(lab6-simul PUBLIC __SYSTEM_CLOCK=48000000)
