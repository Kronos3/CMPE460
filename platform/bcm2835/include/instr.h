#ifndef CMPE460_INSTR_H
#define CMPE460_INSTR_H

extern void enable_irq(void);
extern void disable_irq(void);

/**
 * Mask all interrupts on the CPU
 */
#define DISABLE_INTERRUPTS disable_irq

/**
 * Unmask all interrupts (that were previously masked)
 */
#define ENABLE_INTERRUPTS enable_irq

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
