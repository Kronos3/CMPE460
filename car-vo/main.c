#include "i2c.h"
#include "interrupt.h"
#include "cpu.h"
#include <drv/pwm.h>
#include <drv/uart.h>
#include <lib/uartlib.h>
#include <drv/msp432p4/led.h>

#define SLAVE_ADDRESS (0x48)

// P2.4
static const PwmPin steering_motor = {
        MSP432_PWM_0,
        MSP432_PWM_PIN_1
};

// 2 byte granularity
static U8 rx_data[3] = {0};
static I32 rx_position = 0;
static bool_t running = FALSE;

int main()
{
    uart_init(UART_USB, 115200);
    led_init(LED2_BLUE);

    // Servo motor runs with 50Hz base frequency
    pwm_init(steering_motor, 50);
    pwm_set(steering_motor, 0.0);

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    GpioPin scl = GPIO_PIN(1, 6);
    GpioPin sda = GPIO_PIN(1, 7);
    gpio_init(scl, GPIO_FUNCTION_PRIMARY);
    gpio_init(sda, GPIO_FUNCTION_PRIMARY);

    /* eUSCI I2C Slave Configuration */
    I2C_initSlave(EUSCI_B0_BASE, SLAVE_ADDRESS, EUSCI_B_I2C_OWN_ADDRESS_OFFSET0,
                  EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    /* Set in receive mode */
    I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);

    /* Enable the module and enable interrupts */
    I2C_enableModule(EUSCI_B0_BASE);
    I2C_clearInterruptFlag(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_EUSCIB0);
    Interrupt_enableMaster();

    led_on(LED2_BLUE);

    /* Sleeping while not in use */
    while (1)
    {
        // Place CPU in low power mode 0

        /* If we are in the middle of a state transition, keep trying */
        if (BITBAND_PERI(PCM->CTL1, PCM_CTL1_PMR_BUSY_OFS))
            continue;

        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

        // Wait for interrupt
        CPU_wfi();
    }
}

void dispatch_command(void)
{
    if (!rx_data[0])
    {
        // Stop command
        pwm_stop(steering_motor);
        running = FALSE;
        return;
    }

    // Deserialize this data into a raw U16
    U16 raw = (U16) (rx_data[2] << 0)
            | (U16) (rx_data[1] << 8);

    F64 position = (F64)raw / UINT16_MAX;
    pwm_set(steering_motor, position);

    uprintf("%f\r\n", position);

    if (!running)
    {
        pwm_start(steering_motor);
        running = TRUE;
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
        rx_data[rx_position++] = I2C_slaveGetData(EUSCI_B0_BASE);

        /* Resetting the index if we are at the end */
        if (rx_position == sizeof(rx_data))
        {
            dispatch_command();
            rx_position = 0;
        }
    }
}
