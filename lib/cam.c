#include <lib/cam.h>
#include <drv/gpio.h>
#include <drv/adc.h>

#include <instr.h>

// Run the camera clock at 100kHz
#define CAMERA_FREQUENCY (100000.0)

// Run the timer at double the frequency of the clock to set CLK HIGH and LOW
#define CLK_TOGGLE_FREQUENCY (CAMERA_FREQUENCY * 2)

typedef enum
{
    CAM_IDLE,           //!< Waiting for camera request
    CAM_PULSE,          //!< During SI pulse
    CAM_SCAN,           //!< Reading from ADC
} CameraState;

static struct
{
    GpioPin clk;
    GpioPin si;
    tim_t clk_timer;
    tim_t si_timer;
} cam_settings;

static struct
{
    CameraLine* output_buffer;      //!< ADC sample output buffer
    GblReply reply;                 //!< Reply to send when we are finished
    U32 i;                          //!< State specific tick counter
    CameraState state;
} cam_request;

static struct
{
    CameraLine* output_buffer;
    GblReply reply;
} cam_process_state;

static void cam_clear_request(void)
{
    // Clear the request
    cam_request.i = 0;
    cam_request.output_buffer = NULL;
    cam_request.state = CAM_IDLE;
    gbl_reply_clear(&cam_request.reply);
}

static void cam_clear_process(void)
{
    cam_process_state.output_buffer = NULL;
    gbl_reply_clear(&cam_process_state.reply);
}

void cam_init(GpioPin clk, GpioPin si, tim_t clk_timer)
{
    cam_settings.clk = clk;
    cam_settings.si = si;
    cam_settings.clk_timer = clk_timer;

    // Initialize the camera request memory
    cam_clear_request();
    cam_clear_process();

    // Run the systick timer twice as fast as the clock frequency
    // We need double to set the CLK signal low and high
    tim_init(cam_settings.clk_timer, cam_irq, CLK_TOGGLE_FREQUENCY);

    // Initialize the ADC to read from the camera
    adc_init();
}

static void cam_tick_pulse(void)
{
    // The pulse task runs over multiple ticks on the timer
    // The ticks works as follows:
    //   1. SI Pulse to trigger exposure
    //   2. Clock goes high
    //   3. Stop SI Pulse - clock is stopped during scan
    switch(cam_request.i)
    {
        case 0:
            // Start SI Pulse
            gpio_output(cam_settings.si, TRUE);
            gpio_output(cam_settings.clk, FALSE);
            break;
        case 1:
            // Set clock high
            gpio_output(cam_settings.si, TRUE);
            gpio_output(cam_settings.clk, TRUE);
            break;
        case 2:
            // Stop SI Pulse
            gpio_output(cam_settings.si, FALSE);
            gpio_output(cam_settings.clk, TRUE);
            break;
        default:
            FW_ASSERT(0 && "Invalid exposure index", cam_request.i);
    }

    cam_request.i++;

    if (cam_request.i >= 3)
    {
        // We are done with the exposure stage
        // Go to the scan phase
        cam_request.i = 0;
        cam_request.state = CAM_SCAN;
    }
}

static void cam_tick_scan(void)
{
    // Make sure our for condition loop holds
    FW_ASSERT(cam_request.i < CAMERA_BUF_N * 2);
    FW_ASSERT(cam_request.output_buffer);

    // Poll the ADC before the next rising edge
    if (cam_request.i % 2)
    {
        cam_request.output_buffer->data[cam_request.i / 2] = adc_in();
    }

    // Toggle the clock signal
    gpio_output(cam_settings.clk, cam_request.i % 2);

    // Increment the counter
    cam_request.i++;

    if (cam_request.i >= CAMERA_BUF_N * 2)
    {
        // Scanning has finished
        // Send the reply and stop the request

        // Save the data we need before clearing the camera request
        PXX out_buf = (PXX)cam_request.output_buffer;
        GblReply reply_buf = cam_request.reply;

        gpio_output(cam_settings.clk, FALSE);

        // Reset the request state
        // Do this before the reply so that another camera
        // request can be triggered inside the reply
        cam_clear_request();

        // Send the reply
        gbl_reply_ret(&reply_buf,
                      GBL_STATUS_SUCCESS,
                      1, out_buf);
    }
}

void cam_irq(void)
{
    switch(cam_request.state)
    {
        case CAM_IDLE:
            // No running camera requests
            // Stop the systick
            gpio_output(cam_settings.clk, FALSE);
            gpio_output(cam_settings.si, FALSE);
            tim_stop(cam_settings.clk_timer);
            break;
        case CAM_PULSE:
            cam_tick_pulse();
            break;
        case CAM_SCAN:
            cam_tick_scan();
            break;
    }
}

void cam_sample(CameraLine* dest, GblReply reply)
{
    // Make sure there is no running request
    FW_ASSERT(cam_request.state == CAM_IDLE && "Running camera request",
              cam_request.state, cam_request.i);

    // Place the camera request in a start configuration
    cam_request.output_buffer = dest;
    cam_request.reply = reply;
    cam_request.i = 0;
    cam_request.state = CAM_PULSE;

    // Start sampling from the camera by starting the timer
    tim_start(cam_settings.clk_timer);
}

/**
 * This is the integration handler from TIM32_1
 * We will request another image which will reply
 * back to the user when the image is complete.
 * @param reply reply sent by the camera process
 * @param status reply status from the camera
 */
static void camera_process_handler()
{
    cam_sample(cam_process_state.output_buffer,
               cam_process_state.reply);
}

void cam_process(CameraLine* dest,
                 F64 integration_period,
                 tim_t si_timer,
                 GblReply reply)
{
    DISABLE_INTERRUPTS();

    cam_settings.si_timer = si_timer;

    // Disable both of the timers
    tim_stop(cam_settings.clk_timer);
    tim_stop(cam_settings.si_timer);

    tim_init(si_timer, camera_process_handler, 1 / integration_period);

    FW_ASSERT(dest);

    cam_process_state.reply = reply;
    cam_process_state.output_buffer = dest;

    ENABLE_INTERRUPTS();
}

void cam_process_start(void)
{
    FW_ASSERT(cam_process_state.output_buffer && "No process initialized");
    tim_start(cam_settings.si_timer);
}

bool_t cam_process_stop(GblReply reply_on_cancel)
{
    FW_ASSERT(cam_process_state.output_buffer && "No process running");
    tim_stop(cam_settings.si_timer);

    if (cam_request.state == CAM_IDLE)
    {
        return FALSE;
    }
    else
    {
        // Override the currently running request
        cam_request.reply = reply_on_cancel;
        return TRUE;
    }
}
