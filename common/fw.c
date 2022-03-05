#include <stdarg.h>
#include <fw.h>
#include <instr.h>

I32 nop_handler(const char* fmt, ...)
{
    (void) nop_handler;
    (void) fmt;
    return 0;
}

#ifndef __uartlib_LINKED__
extern I32 uprintf(const char* fmt, ...) __attribute__((weak,alias("nop_handler")));
#else
#include <lib/uartlib.h>
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
