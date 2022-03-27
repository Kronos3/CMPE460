#include "i2c.h"
#include "interrupt.h"
#include "cpu.h"
#include <drv/uart.h>
#include <lib/uartlib.h>
#include <drv/msp432p4/led.h>
#include <lib/steering.h>
#include <lib/dc.h>
#include <drv/msp432p4/switch.h>

#define SLAVE_ADDRESS (0x48)

// DC Motors run @ 10 kHz
#define DC_FREQ (10000.0)

// Servo motor runs @ 50 Hz
#define SERVO_FREQ (50.0)

#define COMMAND_QUEUE_N (32)

void dispatch_command(void);


typedef struct
{
    DcParam left;
    DcParam right;
    SteeringParams steering;
} CarParams;

typedef enum {
    STOP = 0,
    SERVO = 1,
    DC0_FORWARD = 2,
    DC0_BACKWARD = 3,
    DC1_FORWARD = 4,
    DC1_BACKWARD = 5,
} opcode_t;

typedef struct {
    opcode_t opcode;
    U8 value;
} Command;

struct {
    Command q[COMMAND_QUEUE_N];
    volatile U32 tail;
    U32 head;
} queue;

static Command rx_data;
static I32 rx_position = 0;

static const CarParams main_params = {
        .left = {
                // P2.4
                .backward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_1
                },
                // P2.5
                .forward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_2
                },
                .enable = GPIO_PIN(3, 6),
                .base_frequency=DC_FREQ
        },
        .right = {
                // P2.6
                .forward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_3
                },
                // P2.7
                .backward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_4
                },
                .enable = GPIO_PIN(3, 7),
                .base_frequency=DC_FREQ
        },
        .steering = {
                // P5.6
                .servo_pin={
                        MSP432_PWM_2,
                        MSP432_PWM_PIN_1
                },
                .servo_pwm_freq=SERVO_FREQ,
//                .left_pwm=.11,
//                .right_pwm=0.04
                .left_pwm=0.10,
                .right_pwm=0.05
        },
};

void car_init(const CarParams* params)
{
    dc_cfg(DC_0, &params->left);
    dc_cfg(DC_1, &params->right);

    // Initialize the motors
    dc_init();
    steering_init(&params->steering);
}

static bool_t car_running = FALSE;

void car_set(bool_t run)
{
    car_running = run;
    if (car_running)
    {
        led2_set(LED2_GREEN);
        dc_start();
        steering_start();
    }
    else
    {
        led2_set(LED2_RED);
        dc_stop();
        steering_stop();
    }
}

void car_toggle()
{
    car_set(!car_running);
}

int main()
{
    uart_init(UART_USB, 115200);
    led_init(LED2_ALL);

    car_init(&main_params);
    switch_init(SWITCH_ALL, SWITCH_INT_PRESS, car_toggle);

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    GpioPin sda = GPIO_PIN(1, 6);
    GpioPin scl = GPIO_PIN(1, 7);
    gpio_init(scl, GPIO_FUNCTION_PRIMARY);
    gpio_init(sda, GPIO_FUNCTION_PRIMARY);

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
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_EUSCIB0);
    Interrupt_enableMaster();

    // Turn off the motors until the switch is pressed
    car_set(FALSE);

    /* Sleeping while not in use */
    while (1)
    {
        // Place CPU in low power mode 0

//        /* If we are in the middle of a state transition, keep trying */
//        if (BITBAND_PERI(PCM->CTL1, PCM_CTL1_PMR_BUSY_OFS))
//            continue;
//
//        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

        // Wait for interrupt
        dispatch_command();
    }
}

void queue_push(const Command* cmd)
{
    queue.tail = (queue.tail + 1) % COMMAND_QUEUE_N;
    queue.q[queue.tail] = *cmd;
}

Command* queue_pop(void)
{
    // Wait for a new command
    while(queue.head == queue.tail);

    Command* out = &queue.q[queue.head];
    queue.head = (queue.head + 1) % COMMAND_QUEUE_N;
    return out;
}

void dispatch_command(void)
{
    Command* cmd = queue_pop();

    led_on(LED2_BLUE);

    switch(cmd->opcode)
    {
        case STOP:
            car_set(FALSE);
            break;
        case SERVO:
        {
            F64 position = cmd->value;
            position /= UINT8_MAX;
            position = (position * 2.0) - 1.0;
            steering_set(position);
        }
            break;
        case DC0_FORWARD:
        {
            F64 value = cmd->value;
            value /= UINT8_MAX;
            dc_set(DC_0, value);
        }
            break;
        case DC0_BACKWARD:
        {
            F64 value = cmd->value;
            value /= UINT8_MAX;
            dc_set(DC_0, -value);
        }
            break;
        case DC1_FORWARD:
        {
            F64 value = cmd->value;
            value /= UINT8_MAX;
            dc_set(DC_1, value);
        }
            break;
        case DC1_BACKWARD:
        {
            F64 value = cmd->value;
            value /= UINT8_MAX;
            dc_set(DC_1, value);
        }
            break;
        default:
            // Ignoring invalid opcode
            uprintf("ERROR OPCODE = %d\r\n", cmd->opcode);
            return;
    }

    uprintf("op=%d val=%d\r\n", cmd->opcode, cmd->value);
    led_off(LED2_BLUE);
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
            queue_push(&rx_data);
            rx_position = 0;
        }
    }
}
