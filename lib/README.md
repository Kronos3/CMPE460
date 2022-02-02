# Libraries

Standalone drivers for certain portions of the MSP432 board.
These drivers may be linked like any other library.

```cmake
build_elf(my_project ${SOURCES})

# Link to modules you need to use
target_link_libraries(my_project oled adc tim uart)
```
