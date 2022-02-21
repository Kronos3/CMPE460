#include <uartlib.h>
#include <cam.h>
#include <switch.h>

// Buffers to swap between image acquisition and processing
static CameraLine camera_buf;

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
}

// We need to expose SysTick_Handler here so that
// the weak reference is overridden
void SysTick_Handler(void)
{
    cam_irq();
}

#define EXPOSURE_TIME (10e-3)

static void single_handler(const GblReply* self, GblStatus status)
{
    (void) self;
    (void) status;

    const U16* camera_buffer = (const U16*) self->ret[0];

    // Print the camera data to the UART
    for (U32 i = 0; i < CAMERA_BUF_N; i++)
    {
        uprintf("%d", camera_buffer[i]);
    }
    uprintf("\r\n");
}

static void switch_handler(void)
{
    static bool_t continuous_mode = TRUE;
    continuous_mode = !continuous_mode;

    switch(continuous_mode)
    {
        default:
        case FALSE:
        {
            // Run a single request
            GblReply single_reply = gbl_reply_init(single_handler, 0);
            if (!cam_process_stop(single_reply))
            {
                // There is no running request
                // We need to send a single one

            }
        }
            break;
        case TRUE:
        {
            cam_process(camera_buf, EXPOSURE_TIME, gbl_reply_init(camera_handler, 0));
            cam_process_start();
        }
            break;
    }
}

int main(void)
{
    uart_init(UART_USB, 9600);
    cam_init();
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);

    // Run the continuous camera process to test
    // on the oscilloscope
    cam_process(camera_buf, EXPOSURE_TIME, gbl_reply_init(camera_handler, 0));
    cam_process_start();

    while(TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}
