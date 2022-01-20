# Bluetooth targets
build_elf(lab4-bt bt/main-bt.c)
target_link_libraries(lab4-bt uart)

build_elf(lab4-bt-2 bt/lab4-bt-2.c)
target_link_libraries(lab4-bt-2 uart led)

build_elf(lab4-bt-3 bt/lab4-bt-3.c)
target_link_libraries(lab4-bt-3 uart)

# I2C targets
