#include <lib/drive.h>
#include <lib/math.h>
#include <lib/oled.h>
#include <lib/drive_math.h>
#include <string.h>

#define CUT_LEFT 20
#define CUT_RIGHT 20

#define PROCESS_FRAME_N MATH_CONVOLVE_N(CAMERA_BUF_N, DRIVE_SMOOTHING_LENGTH)
#define CUT_FRAME_N (PROCESS_FRAME_N - CUT_LEFT - CUT_RIGHT)
#define DERIV_FRAME_N MATH_CONVOLVE_N(CUT_FRAME_N, 2)

static F64 gaussian_normalize[1] = {1.0 / (1 << 14)};
static F64 gaussian_smooth[DRIVE_SMOOTHING_LENGTH];
static F64 gaussian_derivative[2] = {1, -1}; // dx = F[x] - F[x-1]

static F64 casted_frame[CAMERA_BUF_N];
static F64 normalized_frame[CAMERA_BUF_N];
static F64 cut_frame[CUT_FRAME_N];
static F64 smoothed_frame[PROCESS_FRAME_N];
static F64 deriv_frame[DERIV_FRAME_N];
static F64 normalized_deriv[DERIV_FRAME_N];
static F64 smoothed_deriv[DERIV_FRAME_N];

#define FINAL_LENGTH DERIV_FRAME_N
static U8 threshold_frame[FINAL_LENGTH];

static OLEDCanvas canvas;

void drive_math_init(DriveParams* params)
{
    math_gaussian_smoothing(gaussian_smooth, DRIVE_SMOOTHING_LENGTH, params->smoothing_sigma);
}


F64 drive_edge_find(const DriveParams* params, const F64* deriv)
{
    FW_ASSERT(deriv);

    // Find the highest peak
    I32 peak_index = -1;
    for (I32 i = 0; i < DERIV_FRAME_N; i++)
    {
        if (deriv[i] >= params->edge_threshold)
        {
            if (peak_index < 0 || (peak_index >= 0 && deriv[i] > deriv[peak_index]))
            {
                peak_index = i;
            }
        }
    }

    return (F64)(peak_index - (DERIV_FRAME_N / 2.0)) / DERIV_FRAME_N;
}

F64 drive_process_frame(const DriveParams* params, const CameraLine* raw_frame)
{
    // Convert the raw data into F64s
    math_cast(casted_frame, raw_frame->data, CAMERA_BUF_N);

    // Normalize the frame to the maximum output of the ADC
    math_convolve(normalized_frame, casted_frame, gaussian_normalize, CAMERA_BUF_N, 1);

    // Smooth away the noise by applying convolution of smoothing filter
    math_convolve(smoothed_frame, normalized_frame, gaussian_smooth,
                  CAMERA_BUF_N, DRIVE_SMOOTHING_LENGTH);

    // Cut away bad points on the edges
    memcpy(cut_frame, smoothed_frame + CUT_LEFT, sizeof(F64) * CUT_FRAME_N);

    // Find the gradient of the smoothed frame to find edges
//    math_gradient(deriv_frame, smoothed_frame, PROCESS_FRAME_N);
    math_convolve(deriv_frame, cut_frame, gaussian_derivative,
                  CUT_FRAME_N, 2);

    // Derivative experiences corner issues because it has no reference
    // Chop off the ends
    deriv_frame[0] = deriv_frame[1];
    deriv_frame[DERIV_FRAME_N - 1] = deriv_frame[DERIV_FRAME_N - 2];

    // Gain the derivative signal
    F64 amp = 50;
    math_convolve(normalized_deriv, deriv_frame, &amp, DERIV_FRAME_N, 1);

    for(U32 i = 0; i < DERIV_FRAME_N; i++)
    {
        normalized_deriv[i] = FW_ABS(normalized_deriv[i]);
    }

    math_convolve(normalized_deriv, deriv_frame, &amp, DERIV_FRAME_N, 1);

    // Smooth out the derivative
    math_convolve(deriv_frame, normalized_deriv, gaussian_smooth, DERIV_FRAME_N, DRIVE_SMOOTHING_LENGTH);

    oled_floating_to_oled(canvas, normalized_deriv, 0, DERIV_FRAME_N);
    oled_draw(canvas);

    return drive_edge_find(params, deriv_frame);
}
