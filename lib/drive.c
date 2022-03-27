#include <string.h>

#include <lib/math.h>
#include <lib/steering.h>
#include <lib/drive.h>
#include <instr.h>
#include "uartlib.h"
#include "drive_math.h"

static bool_t drive_params_init = FALSE;
static U32 drive_drop_frames = 10;   // drop first 10 frames
static DriveParams drive_params;

// This pointer will be set from the camera reply
// It needs to be copied asynchronously (outside the IRQ)
// We need to copy it so that we don't run into race conditions
// with the camera capture process.
static const CameraLine* volatile frame_ready = NULL;
static U32 frames_dropped = 0;

// This buffer is used to perform the actual processing. It will
// be a copy of the latest frame that has not been dropped
static CameraLine frame_process;


typedef enum
{
    FRAME_EDGE_NONE = 0,
    FRAME_EDGE_LEFT = 1 << 0,
    FRAME_EDGE_RIGHT = 1 << 1,
    FRAME_EDGE_BOTH = FRAME_EDGE_LEFT | FRAME_EDGE_RIGHT,
} frame_edge_t;

typedef struct
{
    F64 distance;
} FrameEdge;

void drive_init(const DriveParams* params)
{
    FW_ASSERT(params);
    FW_ASSERT(!drive_params_init && "Drive already initialized");

    drive_params = *params;
    drive_params_init = TRUE;

    drive_math_init(&drive_params);
}

static CameraLine* drive_next_frame(void)
{
    // Wait for a new frame to come from the camera
    while(!frame_ready);

    // Copy the new frame into the
    DISABLE_INTERRUPTS();
    memcpy(&frame_process, frame_ready, sizeof(CameraLine));
    frame_ready = NULL;
    ENABLE_INTERRUPTS();

    return &frame_process;
}

static drive_state_t
drive_bang_bang_main(F64 edge_distance, const U16* raw_frame)
{
    static F64 turn_amount = 0;
    if (edge_distance == -2)
    {
        U32 sum = 0;
        for (U32 i = 0; i < CAMERA_BUF_N; i++)
        {
            sum += raw_frame[i];
        }

        uprintf("average: %d\r\n", sum / CAMERA_BUF_N);

        if (sum / CAMERA_BUF_N < drive_params.algorithm_params.bang_bang.carpet_thresh)
        {
            static U32 carpet_frame = 0;
            if (++carpet_frame >= drive_params.algorithm_params.bang_bang.stop_frames)
                return DRIVE_FINISHED;
        }
        else
        {
            // Leave turn amount unchanged from last frame
        }
    }
    else if (edge_distance < 0)
    {
        turn_amount = -1 - edge_distance;
    }
    else
    {
        turn_amount = 1 - edge_distance;
    }

    uprintf("\r%f", turn_amount);

    // Driving at a constant speed for bang bang
    drive_set(drive_params.algorithm_params.bang_bang.throttle,
              drive_params.algorithm_params.bang_bang.throttle,
              (-turn_amount) * drive_params.algorithm_params.bang_bang.turning_factor);

    return DRIVE_RUNNING;
}

static drive_state_t
drive_pid_main(F64 edge)
{
    FW_ASSERT(0 && "Not implemented yet!");
}

drive_state_t drive_main(void)
{
    FW_ASSERT(drive_params_init);

    drive_state_t state = DRIVE_RUNNING;
    while(state == DRIVE_RUNNING)
    {
        // Wait for a new frame from the camera
        const CameraLine* raw_frame = drive_next_frame();

        // Process the raw frame to detect edges
        F64 edge = drive_process_frame(&drive_params, raw_frame);

        switch(drive_params.algorithm)
        {
            case DRIVE_BANG_BANG:
                state = drive_bang_bang_main(edge, raw_frame->data);
                break;
            case DRIVE_PID:
                state = drive_pid_main(edge);
                break;
            default:
                FW_ASSERT(0 && "Invalid algorithm", drive_params.algorithm);
        }
    }

    // Stop the motors
    drive_set(0.0, 0.0, 0.0);
    return state;
}

void drive_image_ready(const GblReply* reply, GblStatus status)
{
    // Make sure nothing was corrupted
    FW_ASSERT(reply);
    FW_ASSERT(status == GBL_STATUS_SUCCESS, status);

    // 1. There is already a frame waiting to be processed
    //    We will drop the oldest one, but we should still keep
    //    track of how many framed were dropped
    // 2. Drop the first frame because of invalid data
    if (frame_ready || drive_drop_frames)
    {
        frames_dropped++;
        if (drive_drop_frames) drive_drop_frames--;
        return;
    }

    frame_ready = (CameraLine*) reply->arg[0];
}

void drive_set(F64 throttle_left, F64 throttle_right, F64 steering)
{
    dc_set(drive_params.left, throttle_left);
    dc_set(drive_params.right, throttle_right);
    steering_set(steering);
}
