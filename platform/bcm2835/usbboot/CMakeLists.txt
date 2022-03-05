project(usbboot)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libusb REQUIRED libusb-1.0)

add_executable(usbboot main.c)
target_include_directories(usbboot PRIVATE ${libusb_INCLUDE_DIRS})
target_compile_definitions(usbboot PRIVATE PKG_VER="1.0" GIT_VER="1.0")
target_link_libraries(usbboot ${libusb_LIBRARIES})
