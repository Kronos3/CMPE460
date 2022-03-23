#ifndef CMPE460_DRIVE_H
#define CMPE460_DRIVE_H

#include <lib/dc.h>
#include <lib/cam.h>

//!< Lower to Upper bound (MUST BE ODD)
#define DRIVE_SMOOTHING_LENGTH (7)

// Gaussian filters need to have odd length
COMPILE_ASSERT(DRIVE_SMOOTHING_LENGTH % 2 == 1, gaussian_length);

typedef enum
{
    DRIVE_BANG_BANG,        //!< Rudimentary drive controls
    DRIVE_PID,              //!< Proportional, Integral, Derivative
} drive_algorithm_t;

typedef struct
{
    dc_t left;                      //!< Left DC motor ID
    dc_t right;                     //!< Right DC motor ID
    drive_algorithm_t algorithm;    //!< Driving algorithm
    F64 smoothing_sigma;            //!< Gaussian sigma for smoothing
    F64 derivative_threshold;       //!< Threshold on the edge detection
    union {
        struct {
            // Bang bang is essentially a P-controller meaning
            // we turn away from the closest edge depending on a
            // constant factor that we can set.
            // This algorithm will run the DC motors at a constant
            // speed until no edges are detected. When no edges are
            // detected the car will stop.
            F64 throttle;           //!< Bang Bang! runs at a constant speed (keep this nice and low)
            F64 turning_factor;     //!< Constant factor to turn by depending on distance to edge
        } bang_bang;
        struct {
            F64 kp;                 //!< PID proportional factor
            F64 ki;                 //!< PID integral factor
            F64 kd;                 //!< PID derivative factor
        } pid;
    } algorithm_params;
} DriveParams;

typedef enum
{
    DRIVE_RUNNING,                  //!< We shouldn't exit with this status
    DRIVE_FINISHED,                 //!< Cross the finish line
    DRIVE_NO_TRACK,                 //!< No track detected
} drive_state_t;

/**
 * Initialize the drive process with parameters
 * @param params drive parameters
 */
void drive_init(const DriveParams* params);

/**
 * Run the drive until a finish condition is detected
 * (THIS IS A BLOCKING CALL)
 * @return Finish condition detected
 */
drive_state_t drive_main(void);

/**
 * Tell the drive process an image has finished capturing
 * If this image is comes in while a previous image is
 * processing (i.e. the drive thread is not ready), this
 * frame will be dropped
 * @param frame The new frame that is ready to be processed
 */
void drive_image_ready(const GblReply* reply, GblStatus status);

/**
 * Drive and steer the car all in one place
 * @param throttle_left Left DC motor speed (
 * @param throttle_right Right DC motor speed (-1 backwards, 1 forwards)
 * @param steering Steering position (0 left, 1 right)
 */
void drive_set(F64 throttle, F64 bias, F64 steering);

#endif //CMPE460_DRIVE_H
