#ifndef CMPE460_ARM_H
#define CMPE460_ARM_H

#define DISABLE_INTERRUPTS() __asm__("cpsid if")
#define ENABLE_INTERRUPTS() __asm__("cpsie if")
#define BREAKPOINT() __asm__("bkpt")

#endif //CMPE460_ARM_H
