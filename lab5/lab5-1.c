#include <switch.h>
#include <uart.h>
#include <led.h>

// Override the weak reference default handler
void PORT1_IRQHandler(void)
{
    uart_put(UART_USB, "Button pressed\r\n");

    // Clear all switch interrupts
    switch_clear_interrupt(switch_get_interrupt());
}

int main(void)
{
    uart_init(UART_USB, 9600);
    led_init(LED1_ALL);
    switch_init(SWITCH_1, TRUE);

    while(1);
}
