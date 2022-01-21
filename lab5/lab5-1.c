#include <switch.h>
#include <uartlib.h>
#include <led.h>

// Override the weak reference default handler
void PORT1_IRQHandler(void)
{
    uprintf("Button pressed %d\r\n", switch_get_interrupt());

    // Clear all switch interrupts
    switch_clear_interrupt(switch_get_interrupt());
}

int main(void)
{
    uart_init(UART_USB, 9600);
    led_init(LED1_ALL);
    switch_init(SWITCH_ALL, TRUE);

    while(1);
}
