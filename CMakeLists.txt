set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.20)

# specify cross compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(CMPE460 C ASM)
set(CMAKE_C_STANDARD 99)

include(msp432/platform.cmake)

add_subdirectory(lib)
add_subdirectory(os)

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab4)
add_subdirectory(lab5)
add_subdirectory(lab6)

add_subdirectory(os_test)
