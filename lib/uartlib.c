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
    if (f < 0)
    {
        uart_putchar(uart, '-');
        f = -f;
    }

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

static inline void uart_put_hex(uart_t uart, U32 i, bool_t lower_case)
{
    static const char hex_mappings_l[] = "0123456789abcdef";
    static const char hex_mappings_u[] = "0123456789ABCDEF";
    const char* hex_mappings = lower_case ? hex_mappings_l : hex_mappings_u;

    for (U32 j = 0; j < 8; j++)
    {
        U32 shift_amt = ((7 - j) * 4);
        uart_putchar(uart, hex_mappings[(i >> shift_amt) & 0xF]);
    }
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
                    uart_put_hex(uart, va_arg(args, int), TRUE);
                    break;
                case 'X':
                    uart_put_hex(uart, va_arg(args, int), FALSE);
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

void udebug_dump(const void* data, debug_t type, U32 screen_n, U32 n)
{
    I8* data_8 = (I8*) data;
    I16* data_16 = (I16*) data;
    I32* data_32 = (I32*) data;
    F64* data_f64 = (F64*) data;

    for (U32 i = 0; i < n; i++)
    {
        if (i && i % screen_n == 0)
        {
            uart_put(UART_IO, ", ...\r\n");
        }
        else if (i)
        {
            uart_put(UART_IO, ", ");
        }

        // Checking the type is slightly slower but way cleaner
        switch(type)
        {
            case DEBUG_HEX_16:
                uart_put_hex(UART_IO, data_16[i], TRUE);
                break;
            case DEBUG_HEX_32:
                uart_put_hex(UART_IO, data_32[i], TRUE);
                break;
            case DEBUG_DEC_16:
                uart_put_dec(UART_IO, data_16[i], ' ', 0);
                break;
            case DEBUG_DEC_8:
                uart_put_dec(UART_IO, data_8[i], ' ', 0);
                break;
            case DEBUG_DEC_32:
                uart_put_dec(UART_IO, data_32[i], ' ', 0);
                break;
            case DEBUG_FLOAT_LARGE:
                uart_put_dec(UART_IO, (I32)data_f64[i], ' ', 0);
                break;
            case DEBUG_FLOAT_SMALL:
                uart_put_floating(UART_IO, data_f64[i]);
                break;
            default:
                FW_ASSERT(0 && "Invalid debug type", type);
        }
    }

    uprintf("\r\n");
}
