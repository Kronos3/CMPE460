set(PLATFORM_PATH ${CMAKE_CURRENT_LIST_DIR})
set(LINKER_SCRIPT ${PLATFORM_PATH}/linker.ld)

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
)

add_compile_definitions(
        __BCM2835__
)

add_compile_options(
#        --target=arm-linux-gnueabihf
        -mcpu=arm1176jzf-s -mfpu=vfp
        -nostdlib -nostartfiles -ffreestanding
#        -nobuiltininc  -nostdlibinc
)

add_link_options(
        -Wl,-T,${LINKER_SCRIPT}
#        --target=arm-linux-gnueabihf
        -mcpu=arm1176jzf-s -mfpu=vfp
        -static
        -nostartfiles -ffreestanding
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
            ${PLATFORM_PATH}/system_bcm2835.c
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
            COMMENT "Building ${SREC_FILE}\nBuilding ${IMG_FILE}")

    target_link_options(${NAME} PRIVATE
            "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${NAME}.map")
endfunction()
