#include <drv/msp432p401r/switch.h>
#include <drv/adc.h>
#include <lib/uartlib.h>

// 20mv per degree celsius
#define V_PER_C (0.010)

// 500 mv at 25C
#define V_ZERO_OFFSET (0.750)

// 25 C is base voltage
#define C_ZERO_OFFSET (25.0)

static F64 to_temp_c(F64 voltage)
{
    return ((voltage - V_ZERO_OFFSET) / V_PER_C) + C_ZERO_OFFSET;
}

static F64 c_to_f(F64 celsius)
{
    return celsius * (9.0 / 5.0) + 32.0;
}

static void get_temp(void)
{
    U32 adc_raw = adc_in();
    F64 voltage = adc_voltage(adc_raw);
    F64 c = to_temp_c(voltage);
    F64 f = c_to_f(c);

    uprintf("ADC raw: %d\r\n"
            "Voltage: %f V\r\n"
            "Celsius: %f C\r\n"
            "Fahrenheit: %f F\r\n"
            "\r\n",
            adc_raw, voltage,
            c, f);
}

int main(void)
{
    uart_init(UART_USB, 9600);
    adc_init();
    switch_init(SWITCH_1, SWITCH_INT_PRESS, get_temp);
}
