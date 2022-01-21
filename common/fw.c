
#include <stdio.h>
#include <stdarg.h>
#include <fw.h>
#include <uartlib.h>

I32 nop_handler(const char* fmt, ...)
{
    (void) nop_handler;
    (void) fmt;
    return 0;
}

#ifndef __uartlib_LINKED__
extern I32 uprintf(const char* fmt, ...) __attribute__((weak,alias("nop_handler")));
#endif

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
    // Mask all interrupts
    // Assertion reached, nothing else should run
    __asm__ ("CPSID I");

    uprintf("Assertion failed %s:%lu : (%s)",
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
    __asm__("BKPT");
    while (1)
    {
    }
}