#include "uartlib.h"

static inline void printf_string(
        uart_t uart,
        const char* string)
{
    for (; *string; string++)
    {
        uart_putchar(uart, *string);
    }
}

static inline void uart_put_udec(
        uart_t uart, unsigned i,
        char pad_char, U8 pad_amt)
{
    char buf_[10];
    char* stack_position = buf_;
    if (!i)
    {
        *(stack_position++) = '0';
    }

    while (i)
    {
        *(stack_position++) = '0' + (i % 10);
        i /= 10;
    }

    if ((stack_position - buf_) < pad_amt)
    {
        for (U32 j = 0; j < (pad_amt - (stack_position - buf_)); j++)
        {
            uart_putchar(uart, pad_char);
        }
    }

    while (stack_position != buf_)
    {
        uart_putchar(uart, *(--stack_position));
    }
}
static inline void uart_put_dec(
        uart_t uart, int i,
        char pad_char, U8 pad_amt)
{
    if (i < 0)
    {
        uart_putchar(uart, '-');
        i *= -1;
    }

    uart_put_udec(uart, i, pad_char, pad_amt);
}

static inline void uart_put_floating(uart_t uart, double f)
{
    uart_put_dec(uart, (int) f, '0', 0);
    f -= (int) f;

    uart_putchar(uart, '.');
    for (int i = 0; i < 6; i++)
    {
        f *= 10;
        uart_putchar(uart, ((int) f) + '0');
        f -= (int) f;
    }
}

static inline void uart_put_hex(uart_t uart, int i, char base_hex_a)
{
    // TODO(tumbar) Make this not shitty
    uart_putchar(uart, base_hex_a + ((i & 0xF0000000) >> 28));
    uart_putchar(uart, base_hex_a + ((i & 0x0F000000) >> 24));
    uart_putchar(uart, base_hex_a + ((i & 0x00F00000) >> 20));
    uart_putchar(uart, base_hex_a + ((i & 0x000F0000) >> 16));
    uart_putchar(uart, base_hex_a + ((i & 0x0000F000) >> 12));
    uart_putchar(uart, base_hex_a + ((i & 0x00000F00) >> 8));
    uart_putchar(uart, base_hex_a + ((i & 0x000000F0) >> 4));
    uart_putchar(uart, base_hex_a + ((i & 0x0000000F) >> 0));
}

I32 uvfprintf(
        uart_t uart,
        const char* const format,
        va_list args)
{
    const char* iter = format;
    while (*iter)
    {
        if (*iter == '%')
        {
            char pad_char = 0;
            U8 pad_amt = 0;

            iter++;
            switch (*iter)
            {
                case 's':
                    printf_string(uart, va_arg(args, const char*));
                    break;
                case 'c':
                    uart_putchar(uart, va_arg(args, int));
                    break;
                case 'd':
                case 'i':
                    uart_put_dec(uart, va_arg(args, int), pad_char, pad_amt);
                    break;
                case 'u':
                    uart_put_udec(uart, va_arg(args, unsigned), pad_char, pad_amt);
                    break;
                case 'x':
                    uart_put_hex(uart, va_arg(args, int), 'a');
                    break;
                case 'X':
                    uart_put_hex(uart, va_arg(args, int), 'A');
                    break;
                case 'f':
                    uart_put_floating(uart, va_arg(args, double));
                    break;
                default:
                    pad_char = *(iter++);
                    pad_amt = (*(iter++) - '0');
                    {
                        if (*iter == 'd' || *iter == 'i')
                        {
                            uart_put_dec(uart, va_arg(args, int), pad_char, pad_amt);
                        }
                        else if (*iter == 'u')
                        {
                            uart_put_udec(uart, va_arg(args, int), pad_char, pad_amt);
                        }
                    }
                    FW_ASSERT(pad_amt <= 9, pad_amt);
                    break;
            }
        }
        else
        {
            uart_putchar(uart, *iter);
        }

        iter++;
    }

//    uart_flush(uart);
    return iter - format;
}



I32 uprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    I32 out = uvfprintf(UART_IO, fmt, args);
    va_end(args);
    return out;
}


int ufprintf(uart_t uart, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    I32 out = uvfprintf(uart, fmt, args);
    va_end(args);
    return out;
}
