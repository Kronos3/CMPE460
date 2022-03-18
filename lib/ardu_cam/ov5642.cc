#include <lib/ardu_cam/ardu_cam.h>
#include <lib/ardu_cam/ov5642_regs.h>

OV5642::OV5642(GpioPin cs_pin_, i2c_mux_t mux, ImgType format)
: ArduCam(0x3C, mux, cs_pin_), m_fmt(format)
{
}

void OV5642::init()
{
    ArduCam::init();

    // Common Camera initialization
    ws_16_8(0x3008, 0x80);

    switch (m_fmt)
    {
        case BMP:
            U8 reg_val;
            ws_16_8(0x4740, 0x21);
            ws_16_8(0x501e, 0x2a);
            ws_16_8(0x5002, 0xf8);
            ws_16_8(0x501f, 0x01);
            ws_16_8(0x4300, 0x61);
            rs_16_8(0x3818, &reg_val);
            ws_16_8(0x3818, (reg_val | 0x60) & 0xff);
            rs_16_8(0x3621, &reg_val);
            ws_16_8(0x3621, reg_val & 0xdf);
            break;
        case JPEG:
            ws_16_8(OV5642_QVGA_Preview);
            sleep_ms(100);
            ws_16_8(OV5642_JPEG_Capture_QSXGA);
            ws_16_8(ov5642_320x240);
            sleep_ms(100);
            ws_16_8(0x3818, 0xa8);
            ws_16_8(0x3621, 0x10);
            ws_16_8(0x3801, 0xb0);
            ws_16_8(0x4407, 0x04);
            break;
        case RAW:
            //Init and set 640x480;
            ws_16_8(OV5642_1280x960_RAW);
            ws_16_8(OV5642_640x480_RAW);
            break;
    }
}

void OV5642::set_jpeg_size(JpegSize size)
{
    switch (size)
    {
        case DIMENSION_320x240:
            ws_16_8(ov5642_320x240);
            break;
        case DIMENSION_640x480:
            ws_16_8(ov5642_640x480);
            break;
        case DIMENSION_1024x768:
            ws_16_8(ov5642_1024x768);
            break;
        case DIMENSION_1280x960:
            ws_16_8(ov5642_1280x960);
            break;
        case DIMENSION_1600x1200:
            ws_16_8(ov5642_1600x1200);
            break;
        case DIMENSION_2048x1536:
            ws_16_8(ov5642_2048x1536);
            break;
        case DIMENSION_2592x1944:
            ws_16_8(ov5642_2592x1944);
            break;
        default:
            ws_16_8(ov5642_320x240);
            break;
    }
}

void OV5642::set_light_mode(LightMode light_mode)
{
    switch (light_mode)
    {
        case ADVANCED_AWB:
            ws_16_8(0x3406, 0x0);
            ws_16_8(0x5192, 0x04);
            ws_16_8(0x5191, 0xf8);
            ws_16_8(0x518d, 0x26);
            ws_16_8(0x518f, 0x42);
            ws_16_8(0x518e, 0x2b);
            ws_16_8(0x5190, 0x42);
            ws_16_8(0x518b, 0xd0);
            ws_16_8(0x518c, 0xbd);
            ws_16_8(0x5187, 0x18);
            ws_16_8(0x5188, 0x18);
            ws_16_8(0x5189, 0x56);
            ws_16_8(0x518a, 0x5c);
            ws_16_8(0x5186, 0x1c);
            ws_16_8(0x5181, 0x50);
            ws_16_8(0x5184, 0x20);
            ws_16_8(0x5182, 0x11);
            ws_16_8(0x5183, 0x0);
            break;
        case SIMPLE_AWB:
            ws_16_8(0x3406, 0x00);
            ws_16_8(0x5183, 0x80);
            ws_16_8(0x5191, 0xff);
            ws_16_8(0x5192, 0x00);
            break;
        case MANUAL_DAY:
            ws_16_8(0x3406, 0x1);
            ws_16_8(0x3400, 0x7);
            ws_16_8(0x3401, 0x32);
            ws_16_8(0x3402, 0x4);
            ws_16_8(0x3403, 0x0);
            ws_16_8(0x3404, 0x5);
            ws_16_8(0x3405, 0x36);
            break;
        case MANUAL_A:
            ws_16_8(0x3406, 0x1);
            ws_16_8(0x3400, 0x4);
            ws_16_8(0x3401, 0x88);
            ws_16_8(0x3402, 0x4);
            ws_16_8(0x3403, 0x0);
            ws_16_8(0x3404, 0x8);
            ws_16_8(0x3405, 0xb6);
            break;
        case MANUAL_CWF:
            ws_16_8(0x3406, 0x1);
            ws_16_8(0x3400, 0x6);
            ws_16_8(0x3401, 0x13);
            ws_16_8(0x3402, 0x4);
            ws_16_8(0x3403, 0x0);
            ws_16_8(0x3404, 0x7);
            ws_16_8(0x3405, 0xe2);
            break;
        case MANUAL_CLOUDY:
            ws_16_8(0x3406, 0x1);
            ws_16_8(0x3400, 0x7);
            ws_16_8(0x3401, 0x88);
            ws_16_8(0x3402, 0x4);
            ws_16_8(0x3403, 0x0);
            ws_16_8(0x3404, 0x5);
            ws_16_8(0x3405, 0x0);
            break;
    }
}

void OV5642::set_color_saturation(Calibration color_saturation)
{
    switch (color_saturation)
    {
        case CAL_PLUS_4:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x80);
            ws_16_8(0x5584, 0x80);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_PLUS_3:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x70);
            ws_16_8(0x5584, 0x70);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_PLUS_2:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x60);
            ws_16_8(0x5584, 0x60);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_PLUS_1:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x50);
            ws_16_8(0x5584, 0x50);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_ZERO:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x40);
            ws_16_8(0x5584, 0x40);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_MINUS_1:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x30);
            ws_16_8(0x5584, 0x30);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_MINUS_2:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x20);
            ws_16_8(0x5584, 0x20);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_MINUS_3:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x10);
            ws_16_8(0x5584, 0x10);
            ws_16_8(0x5580, 0x02);
            break;
        case CAL_MINUS_4:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5583, 0x00);
            ws_16_8(0x5584, 0x00);
            ws_16_8(0x5580, 0x02);
            break;
    }
}

void OV5642::set_brightness(Calibration brightness)
{
    switch (brightness)
    {
        case CAL_PLUS_4:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x40);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_PLUS_3:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x30);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_PLUS_2:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x20);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_PLUS_1:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x10);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_ZERO:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x00);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_MINUS_1:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x10);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x08);
            break;
        case CAL_MINUS_2:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x20);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x08);
            break;
        case CAL_MINUS_3:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x30);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x08);
            break;
        case CAL_MINUS_4:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5589, 0x40);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x558a, 0x08);
            break;
    }
}

void OV5642::set_contrast(Calibration contrast)
{
    switch (contrast)
    {
        case CAL_PLUS_4:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x30);
            ws_16_8(0x5588, 0x30);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_PLUS_3:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x2c);
            ws_16_8(0x5588, 0x2c);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_PLUS_2:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x28);
            ws_16_8(0x5588, 0x28);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_PLUS_1:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x24);
            ws_16_8(0x5588, 0x24);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_ZERO:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x20);
            ws_16_8(0x5588, 0x20);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_MINUS_1:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x1C);
            ws_16_8(0x5588, 0x1C);
            ws_16_8(0x558a, 0x1C);
            break;
        case CAL_MINUS_2:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x18);
            ws_16_8(0x5588, 0x18);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_MINUS_3:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x14);
            ws_16_8(0x5588, 0x14);
            ws_16_8(0x558a, 0x00);
            break;
        case CAL_MINUS_4:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x04);
            ws_16_8(0x5587, 0x10);
            ws_16_8(0x5588, 0x10);
            ws_16_8(0x558a, 0x00);
            break;
    }
}

void OV5642::set_hue(Hue hue)
{
    switch (hue)
    {
        case HUE_PLUS_180:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x80);
            ws_16_8(0x5582, 0x00);
            ws_16_8(0x558a, 0x32);
            break;
        case HUE_PLUS_150:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x6f);
            ws_16_8(0x5582, 0x40);
            ws_16_8(0x558a, 0x32);
            break;
        case HUE_PLUS_120:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x40);
            ws_16_8(0x5582, 0x6f);
            ws_16_8(0x558a, 0x32);
            break;
        case HUE_PLUS_90:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x00);
            ws_16_8(0x5582, 0x80);
            ws_16_8(0x558a, 0x02);
            break;
        case HUE_PLUS_60:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x40);
            ws_16_8(0x5582, 0x6f);
            ws_16_8(0x558a, 0x02);
            break;
        case HUE_PLUS_30:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x6f);
            ws_16_8(0x5582, 0x40);
            ws_16_8(0x558a, 0x02);
            break;
        case HUE_0:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x80);
            ws_16_8(0x5582, 0x00);
            ws_16_8(0x558a, 0x01);
            break;
        case HUE_MINUS_30:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x6f);
            ws_16_8(0x5582, 0x40);
            ws_16_8(0x558a, 0x01);
            break;
        case HUE_MINUS_60:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x40);
            ws_16_8(0x5582, 0x6f);
            ws_16_8(0x558a, 0x01);
            break;
        case HUE_MINUS_90:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x00);
            ws_16_8(0x5582, 0x80);
            ws_16_8(0x558a, 0x31);
            break;
        case HUE_MINUS_120:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x40);
            ws_16_8(0x5582, 0x6f);
            ws_16_8(0x558a, 0x31);
            break;
        case HUE_MINUS_150:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x01);
            ws_16_8(0x5581, 0x6f);
            ws_16_8(0x5582, 0x40);
            ws_16_8(0x558a, 0x31);
            break;
    }
}

void OV5642::set_special_effects(SpecialEffects special_effects)
{
    switch (special_effects)
    {
        case BLUEISH:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x18);
            ws_16_8(0x5585, 0xa0);
            ws_16_8(0x5586, 0x40);
            break;
        case GREENISH:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x18);
            ws_16_8(0x5585, 0x60);
            ws_16_8(0x5586, 0x60);
            break;
        case REDDISH:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x18);
            ws_16_8(0x5585, 0x80);
            ws_16_8(0x5586, 0xc0);
            break;
        case BW:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x18);
            ws_16_8(0x5585, 0x80);
            ws_16_8(0x5586, 0x80);
            break;
        case NEGATIVE:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x40);
            break;
        case SEPIA:
            ws_16_8(0x5001, 0xff);
            ws_16_8(0x5580, 0x18);
            ws_16_8(0x5585, 0x40);
            ws_16_8(0x5586, 0xa0);
            break;
        case NORMAL:
            ws_16_8(0x5001, 0x7f);
            ws_16_8(0x5580, 0x00);
            break;
    }
}

void OV5642::set_exposure_level(ExposureLevel level)
{
    switch (level)
    {
        case MINUS_17_EV:
            ws_16_8(0x3a0f, 0x10);
            ws_16_8(0x3a10, 0x08);
            ws_16_8(0x3a1b, 0x10);
            ws_16_8(0x3a1e, 0x08);
            ws_16_8(0x3a11, 0x20);
            ws_16_8(0x3a1f, 0x10);
            break;
        case MINUS_13_EV:
            ws_16_8(0x3a0f, 0x18);
            ws_16_8(0x3a10, 0x10);
            ws_16_8(0x3a1b, 0x18);
            ws_16_8(0x3a1e, 0x10);
            ws_16_8(0x3a11, 0x30);
            ws_16_8(0x3a1f, 0x10);
            break;
        case MINUS_10_EV:
            ws_16_8(0x3a0f, 0x20);
            ws_16_8(0x3a10, 0x18);
            ws_16_8(0x3a11, 0x41);
            ws_16_8(0x3a1b, 0x20);
            ws_16_8(0x3a1e, 0x18);
            ws_16_8(0x3a1f, 0x10);
            break;
        case MINUS_07_EV:
            ws_16_8(0x3a0f, 0x28);
            ws_16_8(0x3a10, 0x20);
            ws_16_8(0x3a11, 0x51);
            ws_16_8(0x3a1b, 0x28);
            ws_16_8(0x3a1e, 0x20);
            ws_16_8(0x3a1f, 0x10);
            break;
        case MINUS_03_EV:
            ws_16_8(0x3a0f, 0x30);
            ws_16_8(0x3a10, 0x28);
            ws_16_8(0x3a11, 0x61);
            ws_16_8(0x3a1b, 0x30);
            ws_16_8(0x3a1e, 0x28);
            ws_16_8(0x3a1f, 0x10);
            break;
        case EXPOSURE_DEFAULT:
            ws_16_8(0x3a0f, 0x38);
            ws_16_8(0x3a10, 0x30);
            ws_16_8(0x3a11, 0x61);
            ws_16_8(0x3a1b, 0x38);
            ws_16_8(0x3a1e, 0x30);
            ws_16_8(0x3a1f, 0x10);
            break;
        case PLUS_03_EV:
            ws_16_8(0x3a0f, 0x40);
            ws_16_8(0x3a10, 0x38);
            ws_16_8(0x3a11, 0x71);
            ws_16_8(0x3a1b, 0x40);
            ws_16_8(0x3a1e, 0x38);
            ws_16_8(0x3a1f, 0x10);
            break;
        case PLUS_07_EV:
            ws_16_8(0x3a0f, 0x48);
            ws_16_8(0x3a10, 0x40);
            ws_16_8(0x3a11, 0x80);
            ws_16_8(0x3a1b, 0x48);
            ws_16_8(0x3a1e, 0x40);
            ws_16_8(0x3a1f, 0x20);
            break;
        case PLUS_10_EV:
            ws_16_8(0x3a0f, 0x50);
            ws_16_8(0x3a10, 0x48);
            ws_16_8(0x3a11, 0x90);
            ws_16_8(0x3a1b, 0x50);
            ws_16_8(0x3a1e, 0x48);
            ws_16_8(0x3a1f, 0x20);
            break;
        case PLUS_13_EV:
            ws_16_8(0x3a0f, 0x58);
            ws_16_8(0x3a10, 0x50);
            ws_16_8(0x3a11, 0x91);
            ws_16_8(0x3a1b, 0x58);
            ws_16_8(0x3a1e, 0x50);
            ws_16_8(0x3a1f, 0x20);
            break;
        case PLUS_17_EV:
            ws_16_8(0x3a0f, 0x60);
            ws_16_8(0x3a10, 0x58);
            ws_16_8(0x3a11, 0xa0);
            ws_16_8(0x3a1b, 0x60);
            ws_16_8(0x3a1e, 0x58);
            ws_16_8(0x3a1f, 0x20);
            break;
    }
}

void OV5642::set_sharpness(Sharpness sharpness)
{
    switch (sharpness)
    {
        case AUTO_SHARPNESS:
            ws_16_8(0x530A, 0x00);
            ws_16_8(0x530c, 0x0);
            ws_16_8(0x530d, 0xc);
            ws_16_8(0x5312, 0x40);
            break;
        case AUTO_SHARPNESS_1:
            ws_16_8(0x530A, 0x00);
            ws_16_8(0x530c, 0x4);
            ws_16_8(0x530d, 0x18);
            ws_16_8(0x5312, 0x20);
            break;
        case AUTO_SHARPNESS_2:
            ws_16_8(0x530A, 0x00);
            ws_16_8(0x530c, 0x8);
            ws_16_8(0x530d, 0x30);
            ws_16_8(0x5312, 0x10);
            break;
        case MANUAL_SHARPNESS_OFF:
            ws_16_8(0x530A, 0x08);
            ws_16_8(0x531e, 0x00);
            ws_16_8(0x531f, 0x00);
            break;
        case MANUAL_SHARPNESS_1:
            ws_16_8(0x530A, 0x08);
            ws_16_8(0x531e, 0x04);
            ws_16_8(0x531f, 0x04);
            break;
        case MANUAL_SHARPNESS_2:
            ws_16_8(0x530A, 0x08);
            ws_16_8(0x531e, 0x08);
            ws_16_8(0x531f, 0x08);
            break;
        case MANUAL_SHARPNESS_3:
            ws_16_8(0x530A, 0x08);
            ws_16_8(0x531e, 0x0c);
            ws_16_8(0x531f, 0x0c);
            break;
        case MANUAL_SHARPNESS_4:
            ws_16_8(0x530A, 0x08);
            ws_16_8(0x531e, 0x0f);
            ws_16_8(0x531f, 0x0f);
            break;
        case MANUAL_SHARPNESS_5:
            ws_16_8(0x530A, 0x08);
            ws_16_8(0x531e, 0x1f);
            ws_16_8(0x531f, 0x1f);
            break;
    }
}

void OV5642::set_mirror_flip(MirrorFlip mirror_flip)
{
    U8 reg_val;
    switch (mirror_flip)
    {
        case MIRROR:
            rs_16_8(0x3818, &reg_val);
            reg_val = reg_val | 0x00;
            reg_val = reg_val & 0x9F;
            ws_16_8(0x3818, reg_val);
            rs_16_8(0x3621, &reg_val);
            reg_val = reg_val | 0x20;
            ws_16_8(0x3621, reg_val);
            break;
        case FLIP:
            rs_16_8(0x3818, &reg_val);
            reg_val = reg_val | 0x20;
            reg_val = reg_val & 0xbF;
            ws_16_8(0x3818, reg_val);
            rs_16_8(0x3621, &reg_val);
            reg_val = reg_val | 0x20;
            ws_16_8(0x3621, reg_val);
            break;
        case MIRROR_FLIP:
            rs_16_8(0x3818, &reg_val);
            reg_val = reg_val | 0x60;
            reg_val = reg_val & 0xFF;
            ws_16_8(0x3818, reg_val);
            rs_16_8(0x3621, &reg_val);
            reg_val = reg_val & 0xdf;
            ws_16_8(0x3621, reg_val);
            break;
        case MIRROR_NORMAL:
            rs_16_8(0x3818, &reg_val);
            reg_val = reg_val | 0x40;
            reg_val = reg_val & 0xdF;
            ws_16_8(0x3818, reg_val);
            rs_16_8(0x3621, &reg_val);
            reg_val = reg_val & 0xdf;
            ws_16_8(0x3621, reg_val);
            break;
    }
}

void OV5642::set_compress_quality(Compress quality)
{
    switch (quality)
    {
        case HIGH_QUALITY:
            ws_16_8(0x4407, 0x02);
            break;
        case DEFAULT_QUALITY:
            ws_16_8(0x4407, 0x04);
            break;
        case LOW_QUALITY:
            ws_16_8(0x4407, 0x08);
            break;
    }
}

void OV5642::test_pattern(TestPattern pattern)
{
    switch (pattern)
    {
        case COLOR_BAR:
            ws_16_8(0x503d, 0x80);
            ws_16_8(0x503e, 0x00);
            break;
        case COLOR_SQUARE:
            ws_16_8(0x503d, 0x85);
            ws_16_8(0x503e, 0x12);
            break;
        case BW_SQUARE:
            ws_16_8(0x503d, 0x85);
            ws_16_8(0x503e, 0x1a);
            break;
        case DLI:
            ws_16_8(0x4741, 0x4);
            break;
    }
}

