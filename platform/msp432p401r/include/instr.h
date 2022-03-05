#ifndef CMPE460_INSTR_H
#define CMPE460_INSTR_H

/**
 * Mask all interrupts on the CPU
 */
#define DISABLE_INTERRUPTS() __asm__("cpsid if")

/**
 * Unmask all interrupts (that were previously masked)
 */
#define ENABLE_INTERRUPTS() __asm__("cpsie if")

/**
 * Sets a debugger breakpoint
 */
#define BREAKPOINT() __asm__("bkpt")

/**
 * Enters low power sleep mode waiting for interrupt (WFI instruction)
 * Processor waits for next interrupt
 */
#define WAIT_FOR_INTERRUPT() __asm__("WFI")

#endif //CMPE460_INSTR_H
