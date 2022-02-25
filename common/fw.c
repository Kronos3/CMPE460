#include <stdarg.h>
#include <fw.h>

I32 nop_handler(const char* fmt, ...)
{
    (void) nop_handler;
    (void) fmt;
    return 0;
}

#ifndef __uartlib_LINKED__
extern I32 uprintf(const char* fmt, ...) __attribute__((weak,alias("nop_handler")));
#else
#include <uartlib.h>
#endif

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
    // Mask all interrupts
    // Assertion reached, nothing else should run
    DISABLE_INTERRUPTS();

    uprintf("Assertion failed %s:%u : (%s)",
            file, line, expr_str);

    va_list args;
    va_start(args, nargs);
    for (U32 i = 0; i < nargs; i++)
    {
        uprintf(", %d", va_arg(args, int));
    }
    va_end(args);
    uprintf("\r\n");

    // Hang Mr. CPU please
//    BREAKPOINT(); // break point if we are in a debugger
    while (1)
    {
        WAIT_FOR_INTERRUPT();
        // spoilers: it will never come because we masked all interrupts)
        // places the CPU in low power state until reset
    }
}

#define FAULT_HANDLER(t)   \
    __asm volatile(        \
        "isb \n"           \
        "dsb \n"           \
        "tst lr, #4    \n"    /* Check which stack the interrupted function was using */ \
        "ite eq        \n"    /* Set up ARM conditional */ \
        "mrseq r0, msp \n"    /* Move the correct stack address to r0 */ \
        "mrsne r0, psp \n" \
        "mov r1, %0    \n" \
        "bl print_context_and_fault \n"   /* Call load_context_from_stack(sp) */ \
    ::"i" (t) )

typedef enum {
    HARD_FAULT,
    MEM_MANAGE,
    BUS_FAULT,
    USAGE_FAULT,
} fault_t;

typedef struct {
    U32 r0;
    U32 r1;
    U32 r2;
    U32 r3;
    U32 r12;
    U32 lr;
    U32 pc;
    U32 xpsr;
} ContextFrame;

void print_context_and_fault(
        const ContextFrame* ctx,
        fault_t context_type)
{
    (void) print_context_and_fault;
    const char* context_type_name;
    switch (context_type)
    {
        case HARD_FAULT:
            context_type_name = "Hardfault";
            break;
        case MEM_MANAGE:
            context_type_name = "MemManage";
            break;
        case BUS_FAULT:
            context_type_name = "BusFault";
            break;
        case USAGE_FAULT:
            context_type_name = "UsageFault";
            break;
        default:
            context_type_name = "Unhandled";
            break;
    }

    uprintf(
            "\r\n%s:\r\n"
            "  r0: 0x%x\r\n"
            "  r1: 0x%x\r\n"
            "  r2: 0x%x\r\n"
            "  r3: 0x%x\r\n"
            " r12: 0x%x\r\n"
            "  lr: 0x%x\r\n"
            "  pc: 0x%x\r\n"
            "xpsr: 0x%x\r\n",
            context_type_name,
            ctx->r0,
            ctx->r1,
            ctx->r2,
            ctx->r3,
            ctx->r12,
            ctx->lr,
            ctx->pc,
            ctx->xpsr
    );
#define STACK_WORDS 8
#if STACK_WORDS > 0
    uprintf("Stack contents:\r\n");
    const U32* sp = (const U32*)(((const char*)ctx) + sizeof(ContextFrame));
    for (U32 i = 0; i < STACK_WORDS; i++)
    {
        uprintf("%02d    %x\r\n", i, sp[i]);
    }
#endif

    FW_ASSERT(0 && "Hard-fault error pc:", ctx->pc);
}

void HardFault_Handler(void)
{
    FAULT_HANDLER(HARD_FAULT);
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
    FAULT_HANDLER(MEM_MANAGE);
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
    FAULT_HANDLER(BUS_FAULT);
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
    FAULT_HANDLER(USAGE_FAULT);
}
