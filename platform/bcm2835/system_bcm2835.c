#include <fw.h>
#include <bcm2835_int.h>

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

static void irq_handler_range(U32 pending, const U32 base)
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
Interrupt_IRQHandler(void)
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

void __attribute__((interrupt("SWI")))
Yield_IRQHandler(void)
{
}

void __attribute__((interrupt("FIQ")))
Fast_IRQHandler(void)
{
}

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
