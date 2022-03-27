#include <fw.h>
#include <instr.h>

#include <drv/msp432p4/switch.h>

#include <lib/uartlib.h>
#include <lib/dc.h>
#include <lib/cam.h>
#include <lib/drive.h>
#include <lib/steering.h>
#include <lib/oled.h>

// 10 ms exposure time
#define EXPOSURE_TIME (5e-3)

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
    DriveParams drive;
} CarParams;

static const CarParams params = {
        .left = {
                // P2.4
                .backward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_1
                },
                // P2.5
                .forward={
                        MSP432_PWM_0,
                        MSP432_PWM_PIN_2
                },
                .enable = GPIO_PIN(3, 6),
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
                .enable = GPIO_PIN(3, 7),
                .base_frequency=DC_FREQ
        },
        .steering = {
                // P5.6
                .servo_pin={
                        MSP432_PWM_2,
                        MSP432_PWM_PIN_1
                },
                .servo_pwm_freq=SERVO_FREQ,
//                .left_pwm=.11,
//                .right_pwm=0.04
                .left_pwm=0.10,
                .right_pwm=0.05
        },
        .cam = {
                .si = GPIO_PIN(5, 5),
                .clk = GPIO_PIN(5, 4),
                .exposure_time = EXPOSURE_TIME,
                .clk_timer = TIM32_1,
                .exposure_timer = TIM32_2,
        },
        .drive = {
                .algorithm_params = {
                        .bang_bang.throttle = 0.25,
                        .bang_bang.turning_factor = 2,
                        .bang_bang.carpet_thresh = 3000,
                        .bang_bang.stop_frames = 10,
//                        .bang_bang.turning_factor_l = 1,
//                        .bang_bang.turning_factor_r = 1,
                },
                .left = DC_1,
                .right = DC_0,
                .algorithm = DRIVE_BANG_BANG,
                .edge_threshold = 0.5,
                .smoothing_sigma = 2,
//                .center=70
        }
};

void car_init(void)
{
    dc_cfg(DC_0, &params.left);
    dc_cfg(DC_1, &params.right);

    // Initialize the motors
    dc_init();
    steering_init(&params.steering);

    // Initialize the control GPIO pins to general purpose
    gpio_init(params.cam.clk, GPIO_FUNCTION_GENERAL);
    gpio_init(params.cam.si, GPIO_FUNCTION_GENERAL);

    // Both of these control pins are output pins
    gpio_options(params.cam.clk, GPIO_OPTIONS_DIRECTION_OUTPUT);
    gpio_options(params.cam.si, GPIO_OPTIONS_DIRECTION_OUTPUT);

    cam_init(params.cam.clk, params.cam.si, params.cam.clk_timer);

    cam_process(&camera_buf,
                params.cam.exposure_time,
                params.cam.exposure_timer,
                gbl_reply_init(drive_image_ready, 1, &camera_buf));

    drive_init(&params.drive);
}

static bool_t car_running = FALSE;

void car_toggle()
{
    car_running = !car_running;
    if (car_running)
    {
        dc_start();

        steering_start();
    }
    else
    {
        steering_set(0.0);
        dc_stop();
        steering_stop();

        // We don't need a stop reply
//        cam_process_stop(gbl_reply_init(NULL, 0));
    }

}

int main()
{
    uart_init(UART_USB, 115200);
    uprintf("Booting\r\n");
    oled_init();

    car_init(&main_params);

    // Wait for the switch to trigger
    switch_init(SWITCH_1, SWITCH_INT_PRESS, car_toggle);
    cam_process_start();

    // Run the drive
    drive_main();

    // Stop car
    DISABLE_INTERRUPTS();
    car_running = TRUE;
    car_toggle();
    ENABLE_INTERRUPTS();

    while (TRUE)
    {
        WAIT_FOR_INTERRUPT();
    }
}
