
#include <drv/msp432p4/led.h>
#include <stdint.h>
#include <lib/uartlib.h>
#include <instr.h>
#include <string.h>
#include "common.h"

static const CarParams params = {
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

void car_init(void)
{
    dc_cfg(DC_0, &params.left);
    dc_cfg(DC_1, &params.right);

    // Initialize the motors
    dc_init();
    steering_init(&params.steering);
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

void car_toggle(void)
{
    car_set(!car_running);
}

void queue_init(Queue* queue)
{
    FW_ASSERT(queue);
    memset(queue, 0, sizeof(Queue));
}

static void toggle_blue(void)
{
    static bool_t blue_led_on = FALSE;
    blue_led_on = !blue_led_on;
    blue_led_on ? led_on(LED2_BLUE) : led_off(LED2_BLUE);
}

void queue_push(Queue* queue, const Command* cmd)
{
    FW_ASSERT(queue);
    FW_ASSERT(cmd);

    queue->q[queue->tail] = *cmd;
    queue->tail = (queue->tail + 1) % COMMAND_QUEUE_N;
}

const Command* queue_pop(Queue* queue)
{
    FW_ASSERT(queue);

    // Check if there is a command
    while (queue->tail == queue->head)
    {
        // Don't use as much power while we are waiting for a command
        WAIT_FOR_INTERRUPT();
    }

    DISABLE_INTERRUPTS();
    Command* out = &queue->q[queue->head];
    queue->head = (queue->head + 1) % COMMAND_QUEUE_N;
    ENABLE_INTERRUPTS();
    return out;
}

void dispatch_command(const Command* cmd)
{
    FW_ASSERT(cmd);

    toggle_blue();

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
            uprintf("op=%d, val=%d INVALID\r\n", cmd->opcode, cmd->value);
            // Ignoring invalid opcode
            return;
    }

//    uprintf("op=%d, val=%d\r\n", cmd->opcode, cmd->value);
}
