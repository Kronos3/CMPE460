build_elf(bare_minimum test/bare_minimum.c)
target_link_libraries(bare_minimum gpio)

build_elf(test_timer test/test_timer.c)
target_link_libraries(test_timer tim gpio)

build_elf(test_cam test/test_cam.cc)
target_link_libraries(test_cam ardu_cam tim gpio)
