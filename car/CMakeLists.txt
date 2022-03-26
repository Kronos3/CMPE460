build_elf(car main.c)
target_link_libraries(car
        steering
        drive
        uart
        uartlib
        led
        cam
        dc
        oled
        switch
        )
target_compile_definitions(car PUBLIC __SYSTEM_CLOCK=48000000)

build_elf(test_steering test_steering.c)
target_link_libraries(test_steering steering uart uartlib led cam dc)

build_elf(test_drive test_drive.c)
target_link_libraries(test_drive uart uartlib drive)
