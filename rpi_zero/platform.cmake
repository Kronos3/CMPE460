set(PLATFORM_PATH ${CMAKE_CURRENT_LIST_DIR})
set(LINKER_SCRIPT ${PLATFORM_PATH}/linker.ld)

include_directories(
        ${PROJECT_SOURCE_DIR}/common
        ${PLATFORM_PATH}/include
)

add_compile_definitions(
        __RPI_ZERO__
)

add_compile_options(
        -mcpu=arm1176jzf-s -mfpu=vfp
        -nostdlib -nostartfiles -ffreestanding
)

add_link_options(
        -Wl,-T,${LINKER_SCRIPT}
        -mcpu=arm1176jzf-s -mfpu=vfp
        -static
        -nostdlib -nostartfiles -ffreestanding
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
            ${PROJECT_SOURCE_DIR}/common/fw.c
            ${PLATFORM_PATH}/startup.s
    )
    target_link_libraries(${NAME}
            gcc     # gcc runtime library
            c       # libc (standard C library)
            m       # math
            nosys   # don't automatically link libc
    )

    set(SREC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.srec)
    set(IMG_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.img)

    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --srec-forceS3 $<TARGET_FILE:${NAME}> -O srec ${SREC_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NAME}> ${IMG_FILE}
            COMMENT "Building ${HEX_FILE}\nBuilding ${IMG_FILE}")

    target_link_options(${NAME} PRIVATE
            "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${NAME}.map")
endfunction()
