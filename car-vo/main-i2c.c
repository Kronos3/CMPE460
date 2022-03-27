#include "i2c.h"
#include "interrupt.h"
#include "cpu.h"
#include "common.h"
#include <drv/uart.h>
#include <drv/msp432p4/led.h>
#include <drv/msp432p4/switch.h>
#include <lib/uartlib.h>
#include <instr.h>

static Command rx_data;
static I32 rx_position = 0;

void WDT_A_holdTimer(void)
{
    //Set Hold bit
    uint8_t newWDTStatus = (WDT_A->CTL | WDT_A_CTL_HOLD);

    WDT_A->CTL = WDT_A_CTL_PW + newWDTStatus;
}

Queue* glob_queue = NULL;

int main()
{
    WDT_A_holdTimer();

    uart_init(UART_USB, 115200);
    led_init(LED2_ALL);

    car_init();
    switch_init(SWITCH_ALL, SWITCH_INT_PRESS, car_toggle);

    Queue q;
    queue_init(&q);
    glob_queue = &q;

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    GpioPin sda = GPIO_PIN(1, 6);
    GpioPin scl = GPIO_PIN(1, 7);
    gpio_init(sda, GPIO_FUNCTION_PRIMARY);
    gpio_init(scl, GPIO_FUNCTION_PRIMARY);

    gpio_options(sda, GPIO_OPTIONS_DIRECTION_INPUT);
    gpio_options(scl, GPIO_OPTIONS_DIRECTION_INPUT);

    /* eUSCI I2C Slave Configuration */
    I2C_initSlave(EUSCI_B0_BASE,
                  SLAVE_ADDRESS,
                  EUSCI_B_I2C_OWN_ADDRESS_OFFSET0,
                  EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    /* Set in receive mode */
    I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);

    /* Enable the module and enable interrupts */
    I2C_enableModule(EUSCI_B0_BASE);
    I2C_clearInterruptFlag(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
//    Interrupt_enableSleepOnIsrExit();  WHYYYYYYYYYY would the example have this HOLY FUCK
    Interrupt_enableInterrupt(INT_EUSCIB0);
    Interrupt_enableMaster();

    // Turn off the motors until the switch is pressed
    car_set(FALSE);

    uprintf("Ready\r\n");

    while (1)
    {
        // Wait for new command
        // Asynchronously pushed to queue via interrupt
        const Command* cmd = queue_pop(&q);
        dispatch_command(cmd);
    }
}

/******************************************************************************
 * The USCI_B0 data ISR RX vector is used to move received data from the I2C
 * master to the MSP432 memory.
 ******************************************************************************/
void EUSCIB0_IRQHandler(void)
{
    uint_fast16_t status;

    status = I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
    I2C_clearInterruptFlag(EUSCI_B0_BASE, status);

    /* RXIFG */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
        U8 data = I2C_slaveGetData(EUSCI_B0_BASE);
        switch(rx_position)
        {
            case 0:
                rx_data.opcode = data;
                break;
            case 1:
                rx_data.value = data;
                break;
            default:
                FW_ASSERT(0 && "Invalid index", rx_position);
        }

        rx_position++;

        /* Resetting the index if we are at the end */
        if (rx_position == sizeof(rx_data))
        {
//            dispatch_command(&rx_data);
            queue_push(glob_queue, &rx_data);
            rx_position = 0;
        }
    }
}
