#include <cam.h>
#include <tim.h>
#include <gpio.h>
#include <adc.h>
#include <string.h>

// Run the camera clock at 100kHz
#define CAMERA_FREQUENCY (100000.0)

// Run the timer at double the frequency of the clock to set CLK HIGH and LOW
#define SYS_TICK_FREQUENCY (CAMERA_FREQUENCY * 2)

static const GpioPin clk = {GPIO_PORT_5, 1 << 4};
static const GpioPin si = {GPIO_PORT_5, 1 << 5};

typedef enum
{
    CAM_IDLE,           //!< Waiting for camera request
    CAM_PULSE,          //!< During SI pulse
    CAM_SCAN,           //!< Reading from ADC
} CameraState;

static struct
{
    U16* output_buffer;     //!< ADC sample output buffer
    GblReply reply;         //!< Reply to send when we are finished
    U32 i;                  //!< State specific tick counter
    CameraState state;
} cam_request;

static struct
{
    U16* output_buffer;
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

void cam_init(void)
{
    // Initialize the camera request memory
    cam_clear_request();
    cam_clear_process();

    // Run the systick timer twice as fast as the clock frequency
    // We need double to set the CLK signal low and high
    tim_systick_init(tim_calculate_arr(TIM32_PSC_1, SYS_TICK_FREQUENCY));

    // Initialize the control GPIO pins to general purpose
    gpio_init(clk, GPIO_FUNCTION_GENERAL);
    gpio_init(si, GPIO_FUNCTION_GENERAL);

    // Both of these control pins are output pins
    gpio_options(clk, GPIO_OPTIONS_DIRECTION_OUTPUT);
    gpio_options(si, GPIO_OPTIONS_DIRECTION_OUTPUT);

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
            gpio_output(si, TRUE);
            gpio_output(clk, FALSE);
            break;
        case 1:
            // Set clock high
            gpio_output(si, TRUE);
            gpio_output(clk, TRUE);
            break;
        case 2:
            // Stop SI Pulse
            gpio_output(si, FALSE);
            gpio_output(clk, TRUE);
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

    // Poll the ADC before the next rising edge
    if (cam_request.i % 2)
    {
        cam_request.output_buffer[cam_request.i / 2] = adc_in();
    }

    // Toggle the clock signal
    gpio_output(clk, cam_request.i % 2);

    // Increment the counter
    cam_request.i++;

    if (cam_request.i >= CAMERA_BUF_N * 2)
    {
        // Scanning has finished
        // Send the reply and stop the request

        // Save the data we need before clearing the camera request
        PXX out_buf = (PXX)cam_request.output_buffer;
        GblReply reply_buf = cam_request.reply;

        gpio_output(clk, FALSE);

        // Reset the request state
        // Do this before the reply so that another camera
        // request can be triggered inside the reply
        cam_clear_request();

        // Send the reply
        gbl_reply_ret(&reply_buf,
                      GBL_STATUS_SUCCESS,
                      1,
                      out_buf);
    }
}

void cam_irq(void)
{
    switch(cam_request.state)
    {
        case CAM_IDLE:
            // No running camera requests
            // Stop the systick
            gpio_output(clk, FALSE);
            gpio_output(si, FALSE);
            tim_systick_set(FALSE);
            break;
        case CAM_PULSE:
            cam_tick_pulse();
            break;
        case CAM_SCAN:
            cam_tick_scan();
            break;
    }
}

void cam_sample(CameraLine dest, GblReply reply)
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
    tim_systick_set(TRUE);
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
    cam_sample(cam_request.output_buffer,
               cam_process_state.reply);
}

void cam_process(CameraLine dest, F64 integration_period, GblReply reply)
{
    DISABLE_INTERRUPTS();

    // Disable both of the timers
    tim_systick_set(FALSE);
    tim32_set(TIM32_1, FALSE);

    tim32_init(TIM32_1, camera_process_handler,
               tim_calculate_arr(TIM32_PSC_1, 1/integration_period),
               TIM32_PSC_1,
               TIM32_MODE_PERIODIC);

    cam_process_state.reply = reply;
    cam_process_state.output_buffer = dest;

    ENABLE_INTERRUPTS();
}

void cam_process_start(void)
{
    FW_ASSERT(cam_process_state.output_buffer && "No process initialized");
    tim32_set(TIM32_1, TRUE);
}

bool_t cam_process_stop(GblReply reply_on_cancel)
{
    FW_ASSERT(cam_process_state.output_buffer && "No process running");
    tim32_set(TIM32_1, FALSE);

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
