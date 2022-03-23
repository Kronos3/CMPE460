#include <string.h>

#include <lib/math.h>
#include <lib/steering.h>
#include <lib/drive.h>

static bool_t drive_params_init = FALSE;
static DriveParams drive_params;

#define PROCESS_FRAME_N MATH_CONVOLVE_N(CAMERA_BUF_N, DRIVE_SMOOTHING_LENGTH)

// This pointer will be set from the camera reply
// It needs to be copied asynchronously (outside the IRQ)
// We need to copy it so that we don't run into race conditions
// with the camera capture process.
static const CameraLine* volatile frame_ready = NULL;
static U32 frames_dropped = 0;

static F64 gaussian_smooth[DRIVE_SMOOTHING_LENGTH];
static F64 casted_frame[CAMERA_BUF_N];
static F64 smoothed_frame[PROCESS_FRAME_N];
static F64 deriv_frame[PROCESS_FRAME_N - 1];
static U8 threshold_frame[PROCESS_FRAME_N - 1];

// This buffer is used to perform the actual processing. It will
// be a copy of the latest frame that has not been dropped
static CameraLine frame_process;


typedef enum
{
    FRAME_EDGE_NONE = 0,
    FRAME_EDGE_LEFT = 1 << 0,
    FRAME_EDGE_RIGHT = 1 << 1,
} frame_edge_t;

typedef struct
{
    F64 left;
    F64 right;
    frame_edge_t edges;
} FrameEdge;

void drive_init(const DriveParams* params)
{
    FW_ASSERT(params);
    FW_ASSERT(!drive_params_init && "Drive already initialized");

    math_gaussian_smoothing(gaussian_smooth, DRIVE_SMOOTHING_LENGTH, params->smoothing_sigma);

    drive_params = *params;
    drive_params_init = TRUE;
}

FrameEdge drive_edge_find(const U8* edge_frame)
{
    FW_ASSERT(edge_frame);

    FrameEdge edges;
    edges.edges = FRAME_EDGE_NONE;
    U32 center = (PROCESS_FRAME_N - 1) / 2;
    for (U32 i = 0; i < center; i++)
    {
        if (!(edges.edges & FRAME_EDGE_LEFT))
        {
            // We haven't found a left edge yet
            if (edge_frame[center - i])
            {
                edges.left = (F64)i / center;
                edges.edges |= FRAME_EDGE_LEFT;
            }
        }

        if (!(edges.edges & FRAME_EDGE_RIGHT))
        {
            // We haven't found a right edge yet
            if (edge_frame[center + i])
            {
                edges.right = (F64)i / center;
                edges.edges |= FRAME_EDGE_RIGHT;
            }
        }
    }

    return edges;
}

static CameraLine* drive_next_frame(void)
{
    // Wait for a new frame to come from the camera
    while(!frame_ready);

    // Copy the new frame into the
    memcpy(&frame_process, frame_ready, sizeof(CameraLine));
    return &frame_process;
}

static drive_state_t
drive_bang_bang_main(const FrameEdge* edges)
{
    FW_ASSERT(edges);

    // Found out which direction to turn
    // We want to turn away from the closest edge
    F64 distance_closeness;
    bool_t turn_left;
    switch(edges->edges)
    {
        case FRAME_EDGE_NONE:
            // No edges detected!!
            // There is no more track
            // STOP THE CAR!!
            return DRIVE_NO_TRACK;
        case FRAME_EDGE_LEFT:
            turn_left = FALSE;
            distance_closeness = edges->left;
            break;
        case FRAME_EDGE_RIGHT:
            turn_left = TRUE;
            distance_closeness = edges->right;
            break;
        case (FRAME_EDGE_LEFT | FRAME_EDGE_RIGHT):
            // Find the closest edge
            if (edges->left < edges->right)
            {
                // Turn away from the left edge
                turn_left = FALSE;
                distance_closeness = edges->left;
            }
            else
            {
                turn_left = TRUE;
                distance_closeness = edges->right;
            }
            break;
        default:
            FW_ASSERT(0 && "Invalid edge case", edges->edges);
    }

    // Distance closeness is a number between 0 and 1.0
    // 0 is the closest to the center of the camera frame
    // 1 is the furthest away
    // We need to turn more or less depending on how close we are
    F64 turn_amount = (1.0 - distance_closeness) *
            drive_params.algorithm_params.bang_bang.turning_factor;

    F64 steering_out = turn_left ? -turn_amount : turn_amount;

    // Driving at a constant speed for bang bang
    drive_set(drive_params.algorithm_params.bang_bang.throttle,
              drive_params.algorithm_params.bang_bang.throttle,
              steering_out);

    return DRIVE_RUNNING;
}

static drive_state_t
drive_pid_main(const FrameEdge* edges)
{
    FW_ASSERT(0 && "Not implemented yet!");
}

static U8* drive_process_frame(const CameraLine* raw_frame)
{
    // Convert the raw data into F64s
    math_cast(casted_frame, raw_frame->data, CAMERA_BUF_N);

    // Smooth away the noise by applying convolution of smoothing filter
    math_convolve(smoothed_frame, casted_frame, gaussian_smooth,
                  CAMERA_BUF_N, DRIVE_SMOOTHING_LENGTH);

    // Find the gradient of the smoothed frame to find edges
    math_gradient(deriv_frame, smoothed_frame, PROCESS_FRAME_N);

    // One of the samples gets merged in during the derivative
    math_threshold(threshold_frame, deriv_frame,
                   drive_params.derivative_threshold,
                   PROCESS_FRAME_N - 1);
    return threshold_frame;
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
        const U8* edges_frame = drive_process_frame(raw_frame);

        // Find the position of the inner two edges
        FrameEdge edges = drive_edge_find(edges_frame);

        switch(drive_params.algorithm)
        {
            case DRIVE_BANG_BANG:
                state = drive_bang_bang_main(&edges);
                break;
            case DRIVE_PID:
                state = drive_pid_main(&edges);
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

    // There is already a frame waiting to be processed
    // We will drop the oldest one, but we should still keep
    // track of how many framed were dropped
    if (frame_ready)
    {
        frames_dropped++;
    }

    frame_ready = (CameraLine*) reply->ret[0];
}

void drive_set(F64 throttle_left, F64 throttle_right, F64 steering)
{
    dc_set(drive_params.left, throttle_left);
    dc_set(drive_params.right, throttle_right);
    steering_set(steering);
}
