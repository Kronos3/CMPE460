#include <lib/ardu_cam/ardu_cam.h>
#include <lib/ardu_cam/ov2640_regs.h>

OV2640::OV2640(GpioPin cs_pin_, i2c_mux_t mux)
: ArduCam(0x30, mux, cs_pin_)
{
}

void OV2640::init()
{
    ArduCam::init();

    // Initialize the OV2640
    // This camera only works with the JPEG imaging format
    ws_8_8(0xff, 0x01);
    ws_8_8(0x12, 0x80);
    sleep_ms(100);

    ws_8_8(OV2640_JPEG_INIT);
    ws_8_8(OV2640_YUV422);
    ws_8_8(OV2640_JPEG);
    ws_8_8(0xff, 0x01);
    ws_8_8(0x15, 0x00);
    ws_8_8(OV2640_320x240_JPEG);
}

void OV2640::set_jpeg_size(JpegSize size)
{
    switch (size)
    {
        case DIMENSION_160x120:
            ws_8_8(OV2640_160x120_JPEG);
            break;
        case DIMENSION_176x144:
            ws_8_8(OV2640_176x144_JPEG);
            break;
        case DIMENSION_320x240:
            ws_8_8(OV2640_320x240_JPEG);
            break;
        case DIMENSION_352x288:
            ws_8_8(OV2640_352x288_JPEG);
            break;
        case DIMENSION_640x480:
            ws_8_8(OV2640_640x480_JPEG);
            break;
        case DIMENSION_800x600:
            ws_8_8(OV2640_800x600_JPEG);
            break;
        case DIMENSION_1024x768:
            ws_8_8(OV2640_1024x768_JPEG);
            break;
        case DIMENSION_1280x1024:
            ws_8_8(OV2640_1280x1024_JPEG);
            break;
        case DIMENSION_1600x1200:
            ws_8_8(OV2640_1600x1200_JPEG);
            break;
        default:
            ws_8_8(OV2640_320x240_JPEG);
            break;
    }
}

void OV2640::set_special_effects(OV2640::SpecialEffects effect)
{
    switch (effect)
    {
        case ANTIQUE:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x18);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x40);
            ws_8_8(0x7d, 0xa6);
            break;
        case BLUEISH:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x18);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0xa0);
            ws_8_8(0x7d, 0x40);
            break;
        case GREENISH:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x18);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x40);
            ws_8_8(0x7d, 0x40);
            break;
        case REDDISH:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x18);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x40);
            ws_8_8(0x7d, 0xc0);
            break;
        case BW:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x18);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x80);
            ws_8_8(0x7d, 0x80);
            break;
        case NEGATIVE:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x40);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x80);
            ws_8_8(0x7d, 0x80);
            break;
        case BW_NEGATIVE:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x58);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x80);
            ws_8_8(0x7d, 0x80);
            break;
        case NORMAL:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x00);
            ws_8_8(0x7c, 0x05);
            ws_8_8(0x7d, 0x80);
            ws_8_8(0x7d, 0x80);
            break;
    }
}

void OV2640::set_contrast(Calibration contrast)
{
    switch (contrast)
    {
        case PLUS_2:

            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x07);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x28);
            ws_8_8(0x7d, 0x0c);
            ws_8_8(0x7d, 0x06);
            break;
        case PLUS_1:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x07);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x24);
            ws_8_8(0x7d, 0x16);
            ws_8_8(0x7d, 0x06);
            break;
        case ZERO:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x07);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x06);
            break;
        case MINUS_1:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x07);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x2a);
            ws_8_8(0x7d, 0x06);
            break;
        case MINUS_2:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x07);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x18);
            ws_8_8(0x7d, 0x34);
            ws_8_8(0x7d, 0x06);
            break;
    }
}

void OV2640::set_brightness(Calibration brightness)
{
    switch (brightness)
    {
        case PLUS_2:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x09);
            ws_8_8(0x7d, 0x40);
            ws_8_8(0x7d, 0x00);
            break;
        case PLUS_1:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x09);
            ws_8_8(0x7d, 0x30);
            ws_8_8(0x7d, 0x00);
            break;
        case ZERO:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x09);
            ws_8_8(0x7d, 0x20);
            ws_8_8(0x7d, 0x00);
            break;
        case MINUS_1:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x09);
            ws_8_8(0x7d, 0x10);
            ws_8_8(0x7d, 0x00);
            break;
        case MINUS_2:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x04);
            ws_8_8(0x7c, 0x09);
            ws_8_8(0x7d, 0x00);
            ws_8_8(0x7d, 0x00);
            break;
    }
}

void OV2640::set_color_saturation(Calibration color_saturation)
{
    switch (color_saturation)
    {
        case PLUS_2:

            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x02);
            ws_8_8(0x7c, 0x03);
            ws_8_8(0x7d, 0x68);
            ws_8_8(0x7d, 0x68);
            break;
        case PLUS_1:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x02);
            ws_8_8(0x7c, 0x03);
            ws_8_8(0x7d, 0x58);
            ws_8_8(0x7d, 0x58);
            break;
        case ZERO:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x02);
            ws_8_8(0x7c, 0x03);
            ws_8_8(0x7d, 0x48);
            ws_8_8(0x7d, 0x48);
            break;
        case MINUS_1:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x02);
            ws_8_8(0x7c, 0x03);
            ws_8_8(0x7d, 0x38);
            ws_8_8(0x7d, 0x38);
            break;
        case MINUS_2:
            ws_8_8(0xff, 0x00);
            ws_8_8(0x7c, 0x00);
            ws_8_8(0x7d, 0x02);
            ws_8_8(0x7c, 0x03);
            ws_8_8(0x7d, 0x28);
            ws_8_8(0x7d, 0x28);
            break;
    }
}

void OV2640::set_light_mode(LightMode light_mode)
{
    switch (light_mode)
    {
        default:
        case AUTO:
            ws_8_8(0xff, 0x00);
            ws_8_8(0xc7, 0x00); //AWB on
            break;
        case SUNNY:
            ws_8_8(0xff, 0x00);
            ws_8_8(0xc7, 0x40); //AWB off
            ws_8_8(0xcc, 0x5e);
            ws_8_8(0xcd, 0x41);
            ws_8_8(0xce, 0x54);
            break;
        case CLOUDY:
            ws_8_8(0xff, 0x00);
            ws_8_8(0xc7, 0x40); //AWB off
            ws_8_8(0xcc, 0x65);
            ws_8_8(0xcd, 0x41);
            ws_8_8(0xce, 0x4f);
            break;
        case OFFICE:
            ws_8_8(0xff, 0x00);
            ws_8_8(0xc7, 0x40); //AWB off
            ws_8_8(0xcc, 0x52);
            ws_8_8(0xcd, 0x41);
            ws_8_8(0xce, 0x66);
            break;
        case HOME:
            ws_8_8(0xff, 0x00);
            ws_8_8(0xc7, 0x40); //AWB off
            ws_8_8(0xcc, 0x42);
            ws_8_8(0xcd, 0x3f);
            ws_8_8(0xce, 0x71);
            break;
    }
}

