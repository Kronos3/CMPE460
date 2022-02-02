#include <rtos.h>
#include <scheduler.h>
#include <uartlib.h>
#include <tim.h>

static STACK_WORDS(adc_stack, 32);
static OS_Task adc_task;

static STACK_WORDS(uart_stack, 32);
static OS_Task uart_task;

static void adc_main(void* arg)
{
    (void) arg;
    while(1);
}

static void uart_main(void* arg)
{
    (void) arg;

    while(1)
    {
        // Print hello every second
        os_task_delay(1000);
        uprintf("Hello\r\n");
    }
}

static void init(void)
{
    uart_init(UART_USB, 9600);
    tim_systick_init(tim_calculate_arr(1, 1000));
}

int main(void)
{
    init();

    os_task_create(&adc_task, 14, adc_stack, adc_main, NULL);
    os_task_create(&uart_task, 10, uart_stack, uart_main, NULL);

    tim_systick_set(TRUE);

    os_scheduler_main();
}
