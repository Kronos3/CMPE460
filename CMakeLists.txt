set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.20)

# project settings
project(CMPE460 C CXX ASM)
set(CMAKE_C_STANDARD 99)

set(PLATFORM "msp432p4" CACHE STRING "Target board to platform drivers and targets for")
set(BOARD "msp432p4111" CACHE STRING "Board variant for platform")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR})
include(platform/${PLATFORM}/platform.cmake)

function(gen_library name)
    add_library(${name} STATIC ${ARGN})
    target_compile_definitions(${name} PUBLIC __${name}_LINKED__)
endfunction()

add_subdirectory(drv)
add_subdirectory(lib)

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab4)
add_subdirectory(lab5)
add_subdirectory(lab6)
add_subdirectory(lab8)

add_subdirectory(car-vo)
add_subdirectory(car)
