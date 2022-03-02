set(PLATFORM_PATH ${CMAKE_CURRENT_LIST_DIR})
set(LINKER_SCRIPT ${PLATFORM_PATH}/msp432p401r.lds)

set(SDK_PATH /opt/ti/simplelink_msp432p4_sdk_3_40_01_02/)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/sys/CMSIS
        ${PROJECT_SOURCE_DIR}/include/sys/msp
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
        -Wl,-T,${PLATFORM_PATH}/msp432p401r.lds
        -march=armv7e-m
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
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
            ${PLATFORM_PATH}/startup_msp432p401r_gcc.c
            ${PLATFORM_PATH}/system_msp432p401r.c
            ${PLATFORM_PATH}/fw.c
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
