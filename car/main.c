#include <fw.h>
#include <instr.h>
#include <lib/uartlib.h>
#include <lib/dc.h>
#include <lib/cam.h>
#include <lib/drive.h>
#include <lib/steering.h>

// 10 ms exposure time
#define CAM_FPS (100.0)

// DC Motors run @ 10 kHz
#define DC_FREQ (10000.0)

// Servo motor runs @ 50 Hz
#define SERVO_FREQ (50.0)

static CameraLine camera_buf;

typedef struct
{
    GpioPin clk;
    GpioPin si;
    F64 exposure_time;
    tim_t clk_timer;
    tim_t exposure_timer;
} CamParams;

typedef struct
{
    DcParam left;
    DcParam right;
    SteeringParams steering;
    CamParams cam;
} CarParams;

static const CarParams main_params = {
        .left = {
                // P2.4
                .forward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_1
                },
                // P2.5
                .backward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_2
                },
                .base_frequency=DC_FREQ
        },
        .right = {
                // P2.6
                .forward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_3
                },
                // P2.7
                .backward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_4
                },
                .base_frequency=DC_FREQ
        },
        .steering = {
                // P5.6
                .servo_pin={
                        MSP432_PWM_2,
                        MSP432_PWM_PIN_1
                },
                .servo_pwm_freq=SERVO_FREQ,
                .left_pwm=0.03,
                .right_pwm=.11
        },
        .cam = {
                .si = GPIO_PIN(5, 5),
                .clk = GPIO_PIN(5, 4),
                .exposure_time = (1.0 / CAM_FPS),
                .clk_timer = TIM32_1,
                .exposure_timer = TIM32_2,
        }
};

void car_init(const CarParams* params)
{
    dc_cfg(DC_0, &params->left);
    dc_cfg(DC_1, &params->right);

    // Initialize the motors
    dc_init();
    steering_init(&params->steering);

    // Initialize the control GPIO pins to general purpose
    gpio_init(params->cam.clk, GPIO_FUNCTION_GENERAL);
    gpio_init(params->cam.si, GPIO_FUNCTION_GENERAL);

    // Both of these control pins are output pins
    gpio_options(params->cam.clk, GPIO_OPTIONS_DIRECTION_OUTPUT);
    gpio_options(params->cam.si, GPIO_OPTIONS_DIRECTION_OUTPUT);

    cam_init(params->cam.clk, params->cam.si, params->cam.clk_timer);
}

void car_start(const CarParams* params)
{
    dc_start();
    steering_start();

    cam_process(&camera_buf,
                params->cam.exposure_time,
                params->cam.exposure_timer,
                gbl_reply_init(drive_image_ready, 0));

}

int main()
{
    uart_init(UART_USB, 115200);
    car_init(&main_params);

    // Start the car's controlling hardware
    car_start(&main_params);

    // Run the drive
    drive_main();

    // Stop the camera
    // We don't need a reply
    cam_process_stop(gbl_reply_init(NULL, 0));

    while (TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}
