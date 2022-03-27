
#ifndef CMPE460_DRIVE_MATH_H
#define CMPE460_DRIVE_MATH_H

#include <lib/drive.h>

/**
 * Initialize global memory for processing line data
 * @param params
 */
void drive_math_init(DriveParams* params);

/**
 * Given a camera frame find the highest edge of the track
 * @param params drive parameters for edge thresholding
 * @param raw_frame
 * @return
 */
F64 drive_process_frame(const DriveParams* params, const CameraLine* raw_frame);

#endif //CMPE460_DRIVE_MATH_H
