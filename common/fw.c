
#include <stdio.h>
#include <stdarg.h>
#include <fw.h>

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
    // Mask all interrupts
    // Assertion reached, nothing else should run
    __asm__ ("CPSID I");

    printf("Assertion failed %s:%lu : (%s)",
           file, line, expr_str);

    va_list args;
    va_start(args, nargs);
    for (U32 i = 0; i < nargs; i++)
    {
        printf(", %d", va_arg(args, int));
    }
    va_end(args);
    printf("\r\n");

    // Hang Mr. CPU please
    // Flash LEDs using TIM5
    __asm__("BKPT");
    while (1)
    {
        // Create a nice little flashing pattern
        // This cannot be mistaken for some status code
//        set_led_1(TIM5->CNT & (1 << 11));
//        set_led_2(!(TIM5->CNT & (1 << 11)));
//        set_led_3(!(TIM5->CNT & (1 << 11)));
//        set_led_4(TIM5->CNT & (1 << 11));
    }
}