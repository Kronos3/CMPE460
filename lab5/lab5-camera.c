#include <lib/uartlib.h>
#include <lib/cam.h>
#include <drv/msp432p4/switch.h>

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

    GpioPin clk = GPIO_PIN(5, 4);
    GpioPin si = GPIO_PIN(5, 5);

    // Initialize the control GPIO pins to general purpose
    gpio_init(clk, GPIO_FUNCTION_GENERAL);
    gpio_init(si, GPIO_FUNCTION_GENERAL);

    // Both of these control pins are output pins
    gpio_options(clk, GPIO_OPTIONS_DIRECTION_OUTPUT);
    gpio_options(si, GPIO_OPTIONS_DIRECTION_OUTPUT);

    cam_init(clk, si, TIM32_1);
    switch_init(SWITCH_1, SWITCH_INT_PRESS, switch_handler);

    // Run the continuous camera process to test
    // on the oscilloscope
    cam_process(&camera_buf, EXPOSURE_TIME, SYSTICK, gbl_reply_init(camera_handler, 0));

    while(TRUE)
    {
        while(!put_data);
        put_data = 0;

        // Print the camera data to the UART
        uprintf("-1\r\n");
        for (U32 i = 0; i < CAMERA_BUF_N; i++)
        {
            uprintf("%u\r\n", camera_buf.data[i]);
        }
        uprintf("-2\r\n");
    }
}
