#ifndef CMPE460_CAM_H
#define CMPE460_CAM_H

#include <gbl.h>

#ifndef __cam_LINKED__
#error "You need to link 'cam' to use this header"
#endif

#define CAMERA_BUF_N (128)

/**
 * Holds a single line of ADC readings from the line-scan camera
 */
typedef U16 CameraLine[CAMERA_BUF_N];

/**
 * Initialize the peripherals to drive the camera
 */
void cam_init(void);

/**
 * Read samples from the camera
 * @param dest destination buffer
 * @param reply reply to send when camera is done
 */
void cam_sample(CameraLine dest, GblReply reply);

#endif //CMPE460_CAM_H
