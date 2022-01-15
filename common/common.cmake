set(COMMON_PATH ${PROJECT_SOURCE_DIR}/common)
set(LINKER_SCRIPT ${COMMON_PATH}/msp432p401r.lds)

function(build_elf NAME)
    set(SOURCES ${ARGV})
    LIST(REMOVE_AT SOURCES 0)
    add_executable(
            ${NAME}.elf
            ${SOURCES}
            ${LINKER_SCRIPT}
            ${COMMON_PATH}/startup_msp432p401r_gcc.c
            ${COMMON_PATH}/system_msp432p401r.c
    )

    target_link_libraries(${NAME}.elf
            :ti/display/lib/display.am4fg
            :ti/grlib/lib/gcc/m4f/grlib.a
            :third_party/spiffs/lib/gcc/m4f/spiffs.a
            :ti/drivers/lib/drivers_msp432p401x.am4fg
            :third_party/fatfs/lib/gcc/m4f/fatfs.a
            :ti/devices/msp432p4xx/driverlib/gcc/msp432p4xx_driverlib.a

            gcc
            c
            m
            nosys
    )

    set(HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.hex)
    set(BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bin)

    add_custom_command(TARGET ${NAME}.elf POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NAME}.elf> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NAME}.elf> ${BIN_FILE}
            COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}")

    target_link_options(${NAME}.elf PRIVATE
            "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${NAME}.map")

endfunction()
