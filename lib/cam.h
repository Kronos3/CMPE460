#ifndef CMPE460_CAM_H
#define CMPE460_CAM_H

#include <lib/gbl.h>

#include <drv/tim.h>
#include <drv/gpio.h>

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
void cam_init(GpioPin clk, GpioPin si, tim_t clk_timer);

/**
 * Read samples from the camera
 * @param dest destination buffer
 * @param reply reply to send when camera is done
 */
void cam_sample(CameraLine dest, GblReply reply);

/**
 * Exposed camera IRQ that needs to be called from SysTick
 */
void cam_irq(void);

/**
 * Initialize the camera process to continuously integrate
 * with the camera at a certain integration period
 * @param dest buffer to write camera images to
 * @param integration_period time to integrate each image on the camera
 * @param reply reply to call each time an image is ready
 */
void cam_process(CameraLine dest,
                 F64 integration_period,
                 tim_t si_timer,
                 GblReply reply);

/**
 * Start the camera process
 * (must be called after cam_process())
 */
void cam_process_start(void);

/**
 * Stop the camera process
 * @param reply_on_cancel reply to override the currently running camera request (if any)
 * @return TRUE a camera request was canceled and overridden by reply_on_cancel
 *          FALSE if no camera request was running
 */
bool_t cam_process_stop(GblReply reply_on_cancel);

#endif //CMPE460_CAM_H
