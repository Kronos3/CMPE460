gen_library(ardu_cam
        ardu_cam.cc ov2640.cc ov5642.cc
        ardu_cam.h common.h
        ov5642_regs.h ov2640_regs.h)
target_link_libraries(ardu_cam i2c_mux spi gpio)
