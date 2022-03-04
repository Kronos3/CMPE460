build_elf(bare_minimum bare_minimum.c)

build_elf(test_timer test_timer.c)
target_link_libraries(test_timer tim gpio)
