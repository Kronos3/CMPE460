set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.20)

# project settings
project(CMPE460 C ASM)
set(CMAKE_C_STANDARD 99)

set(PLATFORM "msp432p401r" CACHE STRING "Target board to build drivers and targets for")

include_directories(${PROJECT_SOURCE_DIR})
include(platform/${PLATFORM}/platform.cmake)

function(gen_library name)
    add_library(${name} STATIC ${ARGN})
    target_compile_definitions(${name} PUBLIC __${name}_LINKED__)
endfunction()

add_subdirectory(drv)
add_subdirectory(lib)

add_subdirectory(top)
