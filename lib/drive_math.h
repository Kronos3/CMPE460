
#ifndef CMPE460_DRIVE_MATH_H
#define CMPE460_DRIVE_MATH_H

#include <lib/drive.h>

void drive_math_init(DriveParams* params);
F64 drive_edge_find(const DriveParams* params, const F64* deriv);

F64 drive_process_frame(const DriveParams* params, const CameraLine* raw_frame);

#endif //CMPE460_DRIVE_MATH_H
