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

set(SDK_PATH /opt/ti/simplelink_msp432p4_sdk_3_40_01_02/)
set(COMMON_PATH ${CMAKE_CURRENT_LIST_DIR}/common)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/sys/CMSIS
        ${CMAKE_CURRENT_LIST_DIR}/include/sys/msp
)

add_compile_definitions(
        __MSP432P401R__
        DeviceFamily_MSP432P401x
)

add_compile_options(
        -mcpu=cortex-m4
        -march=armv7e-m
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -ffunction-sections
        -fdata-sections
        -g
        -gstrict-dwarf
        -Wall
)

link_directories(${SDK_PATH}/source)

add_link_options(
        -Wl,-T,${COMMON_PATH}/msp432p401r.lds
        -march=armv7e-m
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -static
        -Wl,--gc-sections
        --specs=nano.specs
)

include(${COMMON_PATH}/common.cmake)

add_subdirectory(lib)

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab4)
add_subdirectory(lab5)
