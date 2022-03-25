set(PLATFORM_PATH ${CMAKE_CURRENT_LIST_DIR})

set(SDK_PATH /opt/ti/simplelink_msp432p4_sdk_3_40_01_02/)

# specify cross compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

include_directories(
        ${PROJECT_SOURCE_DIR}/common
        ${PLATFORM_PATH}/include
        ${PLATFORM_PATH}/include/sys/CMSIS
        ${PLATFORM_PATH}/include/sys/msp
)

string(TOUPPER ${PLATFORM} PLATFORM_DEF)
string(TOUPPER ${BOARD} BOARD_DEF)

add_compile_definitions(
        __${PLATFORM_DEF}__
        __${BOARD_DEF}__
)

set(LINKER_SCRIPT ${PLATFORM_PATH}/${BOARD}.lds)

add_compile_options(
        -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
        -ffunction-sections
        -fdata-sections
        -g
        -gstrict-dwarf
        -Wall
)

link_directories(${SDK_PATH}/source)

add_link_options(
        -Wl,-T,${LINKER_SCRIPT}
        -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
        -static
        -Wl,--gc-sections
        --specs=nano.specs
)


function(build_elf NAME)
    set(SOURCES ${ARGV})
    LIST(REMOVE_AT SOURCES 0)
    add_executable(
            ${NAME}
            ${SOURCES}
            ${LINKER_SCRIPT}
            ${PLATFORM_PATH}/startup_${BOARD}_gcc.c
            ${PLATFORM_PATH}/system_${BOARD}.c
            ${PROJECT_SOURCE_DIR}/common/fw.c
            ${PLATFORM_PATH}/syscalls.c
    )
    target_link_libraries(${NAME}
            gcc     # gcc runtime library
            c       # libc (standard C library)
            m       # math
            nosys   # don't automatically link libc
    )

    set(HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.hex)
    set(BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bin)

    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NAME}> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NAME}> ${BIN_FILE}
            COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}")

    target_link_options(${NAME} PRIVATE
            "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${NAME}.map")
endfunction()
