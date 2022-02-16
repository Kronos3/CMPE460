#include <uartlib.h>
#include <cam.h>
#include <oled.h>
#include <switch.h>

// Number of buffers to switch between while imaging and drawing
#define CAMERA_BUF_TOTAL_N (2)

// Whether to continuously scan/draw the camera image
static volatile bool_t imaging_enabled = FALSE;

// Buffer ready to be displayed on the OLED display
static U16* volatile buffer_ready = NULL;

// Buffers to swap between image acquisition and processing
static volatile U32 camera_buf_i = 0;
static CameraLine camera_buf[CAMERA_BUF_TOTAL_N];

// Our processing tasks assumes more than one buffer
COMPILE_ASSERT(CAMERA_BUF_TOTAL_N > 1, number_of_camera_buffers_gt_1);

/**
 * Handle camera replies by notifying the main process
 * Swap the current buffer ptr
 * @param reply reply sent by the camera process
 * @param status reply status from the camera
 */
static void camera_finished(const GblReply* reply, GblStatus status)
{
    FW_ASSERT(status == GBL_STATUS_SUCCESS && "Camera request failed");
    FW_ASSERT(!buffer_ready && "Buffer has not finished processing when a camera reply was received", buffer_ready);

    // Tell the drawing process that a new camera line is ready to draw
    buffer_ready = (U16*) reply->ret[0];

    // Swap to a new buffer
    camera_buf_i = (camera_buf_i + 1) % CAMERA_BUF_TOTAL_N;
}

/**
 * Handle button press by toggling image enable
 */
static void switch_handler(void)
{
    imaging_enabled = !imaging_enabled;

    switch(imaging_enabled)
    {
        case FALSE:
        // Stop imaging on the camera
        // Notify the user that imaging has stopped on the OLED
        {
            static TextCanvas tc = {
                    .scroll = FALSE,
                    .current_line = 1,
                    .canvas = {0}
            };

            oled_clear(tc.canvas);
            oled_print(&tc, "Camera stopped");
            oled_print(&tc, "Press SW1 to start");
        }
            break;
        case TRUE:
        // Start the camera imaging chain
        {
            GblReply cam_reply = gbl_reply_init(camera_finished, 0);
            cam_sample(camera_buf[camera_buf_i], cam_reply);
        }
            break;
        default:
            FW_ASSERT(0 && "Invalid imaging state", imaging_enabled);
    }
}

int main(void)
{
    uart_init(UART_USB, 9600);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);
    oled_init();
    cam_init();

    // We are using the same reply for every chained
    // cam_sample() call
    GblReply cam_reply = gbl_reply_init(camera_finished, 0);

    static TextCanvas tc = {
            .scroll = FALSE,
            .current_line = 1,
            .canvas = {0}
    };

    oled_print(&tc, "Camera not started yet");
    oled_print(&tc, "Press SW1 to start");

    // Canvas used for drawing camera data to OLED
    static OLEDCanvas canvas;

    while(TRUE)
    {
        // Asynchronously set on camera reply
        // First sample is started in the switch interrupt
        if (buffer_ready)
        {
            // Chain the imaging as needed
            // Is done immediately to not waste time waiting for buffer processing
            if (imaging_enabled)
            {
                cam_sample(camera_buf[camera_buf_i], cam_reply);
            }

            // Convert the ADC readings to OLED pixels
            oled_camera_to_oled(canvas, buffer_ready);

            // Draw the canvas to the OLED
            oled_draw(canvas);

            // Clear the waiting buffer
            buffer_ready = NULL;
        }
    }
}
