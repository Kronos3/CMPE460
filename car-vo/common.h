
#ifndef CMPE460_COMMON_H
#define CMPE460_COMMON_H

#include <lib/dc.h>
#include <lib/steering.h>

#define SLAVE_ADDRESS (0x48)

// DC Motors run @ 10 kHz
#define DC_FREQ (10000.0)

// Servo motor runs @ 50 Hz
#define SERVO_FREQ (50.0)

#define COMMAND_QUEUE_N (8)

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

typedef struct {
    volatile U32 tail;
    volatile U32 head;
    Command q[COMMAND_QUEUE_N];
} Queue;

/**
 * Dispatch motor command
 */
void dispatch_command(const Command* command);

void car_init(void);
void car_set(bool_t run);
void car_toggle(void);

void queue_init(Queue* queue);
void queue_push(Queue* queue, const Command* cmd);
const Command* queue_pop(Queue* queue);

#endif //CMPE460_COMMON_H
