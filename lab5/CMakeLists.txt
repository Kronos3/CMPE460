build_elf(lab5-1 lab5-1.c)
target_link_libraries(lab5-1 switch uartlib led tim)
target_compile_definitions(lab5-1 PUBLIC __SYSTEM_CLOCK=48000000)
