#include <fw.h>
#include <bcm2835_int.h>
#include <instr.h>

typedef struct
{
    U32 IRQ_Basic;
    U32 Pending1;
    U32 Pending2;
    U32 FIQCtrl;
    U32 Enable1;
    U32 Enable2;
    U32 EnableBasic;
    U32 Disable1;
    U32 Disable2;
    U32 DisableBasic;
} bcm2835_IntCRegs;

#define INTCREGS   ((volatile bcm2835_IntCRegs*) (BCM2835_BASE_INTC))

static bcm2835_InterruptHandler vector_table[BCM2835_INTC_TOTAL_IRQ] = {NULL};
static U32 enabled_interrupts[3] = {0};

static inline void irq_handler_range(U32 pending, const U32 base)
{
    // Execute all pending interrupts
    while (pending)
    {
        // Get index of first set bit:
        U32 bit = 31 - __builtin_clz(pending);

        // Map to IRQ number:
        U32 irq = base + bit;

        // Call interrupt handler, if enabled:
        if (vector_table[irq])
        {
            vector_table[irq](irq);
        }

        // Clear bit in bitfield:
        pending &= ~(1UL << bit);
    }
}

void __attribute__((interrupt("IRQ")))
interrupt_vector(void)
{
    register U32 ul_masked_status = INTCREGS->IRQ_Basic;

    // Bit 8 in IRQBasic indicates interrupts in Pending1 (interrupts 31-0):
    if (ul_masked_status & (1UL << 8))
    {
        irq_handler_range(INTCREGS->Pending1 & enabled_interrupts[0], 0);
    }

    // Bit 9 in IRQBasic indicates interrupts in Pending2 (interrupts 63-32):
    if (ul_masked_status & (1UL << 9))
    {
        irq_handler_range(INTCREGS->Pending2 & enabled_interrupts[1], 32);
    }

    // Bits 7 through 0 in IRQBasic represent interrupts 64-71:
    if (ul_masked_status & 0xFF)
    {
        irq_handler_range(ul_masked_status & 0xFF & enabled_interrupts[2], 64);
    }
}

// The BCM2835 tim library will be using the fast interrupt for Arm Timer
#ifndef __tim_LINKED__
void __attribute__((interrupt("FIQ")))
fast_interrupt_vector(void)
{
}
#endif

void bcm2835_interrupt_register(bcm2835_Interrupt irq,
                                bcm2835_InterruptHandler handler)
{
    FW_ASSERT(irq < BCM2835_INTC_TOTAL_IRQ, irq);
    vector_table[irq] = handler;
}

void bcm2835_interrupt_enable(bcm2835_Interrupt irq)
{
    FW_ASSERT(irq < BCM2835_INTC_TOTAL_IRQ, irq);
    U32 mask = 1UL << (irq % 32);

    if (irq <= 31)
    {
        INTCREGS->Enable1 = mask;
        enabled_interrupts[0] |= mask;
    }
    else if (irq <= 63)
    {
        INTCREGS->Enable2 = mask;
        enabled_interrupts[1] |= mask;
    }
    else if (irq < BCM2835_INTC_TOTAL_IRQ)
    {
        INTCREGS->EnableBasic = mask;
        enabled_interrupts[2] |= mask;
    }
}

void bcm2835_interrupt_disable(bcm2835_Interrupt irq)
{
    FW_ASSERT(irq < BCM2835_INTC_TOTAL_IRQ, irq);
    U32 mask = 1UL << (irq % 32);

    if (irq <= 31)
    {
        INTCREGS->Disable1 = mask;
        enabled_interrupts[0] &= ~mask;
    }
    else if (irq <= 63)
    {
        INTCREGS->Disable2 = mask;
        enabled_interrupts[1] &= ~mask;
    }
    else if (irq < BCM2835_INTC_TOTAL_IRQ)
    {
        INTCREGS->DisableBasic = mask;
        enabled_interrupts[2] &= ~mask;
    }
}

extern int __bss_start__;
extern int __bss_end__;

extern void _cstartup(void);

void _cstartup(void)
{
    // Enables L1 cache and branch prediction
    start_l1cache();

    int* bss = &__bss_start__;
    int* bss_end = &__bss_end__;

    while (bss < bss_end)
        *bss++ = 0;

    main();

    // Main has exited
    // Trap the CPU
    while (1)
    {
        WAIT_FOR_INTERRUPT();
    }
}

/**
    @brief The Reset vector interrupt handler
    This can never be called, since an ARM core reset would also reset the
    GPU and therefore cause the GPU to start running code again until
    the ARM is handed control at the end of boot loading
*/
void __attribute__((interrupt("ABORT"))) reset_vector(void)
{

}

/**
    @brief The undefined instruction interrupt handler
    If an undefined intstruction is encountered, the CPU will start
    executing this function. Just trap here as a debug solution.
*/
void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void)
{
    while (1)
    {
        /* Do Nothing! */
    }
}


/**
    @brief The supervisor call interrupt handler
    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("SWI"))) software_interrupt_vector(void)
{
    while (1)
    {
        /* Do Nothing! */
    }
}


/**
    @brief The prefetch abort interrupt handler
    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) prefetch_abort_vector(void)
{
}

/**
    @brief The Data Abort interrupt handler
    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) data_abort_vector(void)
{

}
