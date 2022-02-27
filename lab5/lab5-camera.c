#include <uartlib.h>
#include <cam.h>
#include <switch.h>

// Buffers to swap between image acquisition and processing
static CameraLine camera_buf;

#define EXPOSURE_TIME (10e-3)

static volatile bool_t put_data = 0;
/**
 * Handle camera replies by notifying the main process
 * Swap the current buffer ptr
 * @param reply reply sent by the camera process
 * @param status reply status from the camera
 */
static void camera_handler(const GblReply* reply, GblStatus status)
{
    (void) reply;

    FW_ASSERT(status == GBL_STATUS_SUCCESS && "Camera request failed");
    put_data = 1;
}

static void switch_handler(void)
{
    static bool_t running = FALSE;
    running = !running;
    switch(running)
    {
        case TRUE:
            cam_process_start();
            break;
        default:
        case FALSE:
            cam_process_stop(gbl_reply_init(NULL, 0));
            break;
    }
}

int main(void)
{
    uart_init(UART_USB, 115200);
    cam_init();
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);

    // Run the continuous camera process to test
    // on the oscilloscope
    cam_process(camera_buf, EXPOSURE_TIME, gbl_reply_init(camera_handler, 0));

    while(TRUE)
    {
        while(!put_data);
        put_data = 0;

        // Print the camera data to the UART
        uprintf("-1\r\n");
        for (U32 i = 0; i < CAMERA_BUF_N; i++)
        {
            uprintf("%u\r\n", camera_buf[i]);
        }
        uprintf("-2\r\n");
    }
}
