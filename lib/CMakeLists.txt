function(gen_library name)
    add_library(${name} STATIC ${ARGN})
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    target_compile_definitions(${name} PUBLIC __${name}_LINKED__)
endfunction()

gen_library(uart uart.c uart.h)
gen_library(led led.c led.h)
