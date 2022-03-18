#ifndef CMPE460_ARDU_CAM_H
#define CMPE460_ARDU_CAM_H

#include <fw.h>
#include <drv/gpio.h>
#include <lib/ardu_cam/common.h>
#include <lib/i2c_mux.h>

#ifndef __ardu_cam_LINKED__
#error "You need to link 'ardu_cam' to use this header"
#endif

#ifndef __cplusplus
#error "ArduCam needs C++ to be included"
#endif

class ArduCam
{
protected:
    virtual void init();

    /**
     * Create a new ArduCam given the hardware
     * selection made by the user
     * @param addr I2C address of the camera (set by which camera we are using)
     * @param mux_channels I2C multiplexer channel selection
     * @param cs_pin SPI CS pin on the GPIO header
     */
    ArduCam(U8 addr, i2c_mux_t mux_channels, GpioPin cs_pin);

    U8 m_addr;              //!< I2C slave address
    i2c_mux_t m_mux;        //!< I2C mux channel selection
    GpioPin m_cs_pin;       //!< SPI CS Gpio Pin

    void CS_HIGH();

    void CS_LOW();

    void flush_fifo();

    void start_capture();

    void clear_fifo_flag();

    U8 read_fifo();
    void bus_read();

    U8 read_reg(U8 addr);

    void write_reg(U8 addr, U8 data);

    U32 read_fifo_length();

    void set_fifo_burst();

    void set_bit(U8 addr, U8 bit);

    void clear_bit(U8 addr, U8 bit);

    // Write 8 bit values to 8 bit register address
    void ws_8_8(const SensorReg* regs);

    // Write 16 bit values to 8 bit register address
    void ws_8_16(const SensorReg* regs);

    // Write 8 bit values to 16 bit register address
    void ws_16_8(const SensorReg* regs);

    // Write 16 bit values to 16 bit register address
    void ws_16_16(const SensorReg* regs);

    // Read/write 8 bit value to/from 8 bit register address
    void ws_8_8(U8 regID, U8 regDat);
    void rs_8_8(U8 regID, U8* regDat);

    // Read/write 16 bit value to/from 8 bit register address
    void ws_8_16(U8 regID, U16 regDat);
    void rs_8_16(U8 regID, U16* regDat);

    // Read/write 8 bit value to/from 16 bit register address
    void ws_16_8(U16 regID, U8 regDat);
    void rs_16_8(U16 regID, U8* regDat);

    // Read/write 16 bit value to/from 16 bit register address
    void ws_16_16(U16 regID, U16 regDat);
    void rs_16_16(U16 regID, U16* regDat);
};

class OV2640 : public ArduCam
{
public:
    explicit OV2640(GpioPin cs_pin_, i2c_mux_t mux = I2C_MUX_CH_NONE);

    /**
     * Initialize the camera by setting up the
     * I2C, SPI and camera registers
     */
    void init() override;

    //Light Mode
    enum LightMode
    {
        AUTO,
        SUNNY,
        CLOUDY,
        OFFICE,
        HOME,
    };

    // Saturation, Brightness, Contrast
    enum Calibration
    {
        PLUS_2,
        PLUS_1,
        ZERO,
        MINUS_1,
        MINUS_2,
    };

    enum SpecialEffects
    {
        ANTIQUE,
        BLUEISH,
        GREENISH,
        REDDISH,
        BW,
        NEGATIVE,
        BW_NEGATIVE,
        NORMAL,
    };

    enum JpegSize
    {
        DIMENSION_160x120,
        DIMENSION_176x144,
        DIMENSION_320x240,
        DIMENSION_352x288,
        DIMENSION_640x480,
        DIMENSION_800x600,
        DIMENSION_1024x768,
        DIMENSION_1280x1024,
        DIMENSION_1600x1200,
    };

    void set_special_effects(SpecialEffects effect);
    void set_contrast(Calibration contrast);
    void set_brightness(Calibration brightness);
    void set_color_saturation(Calibration color_saturation);
    void set_light_mode(LightMode light_mode);
    void set_jpeg_size(JpegSize size);
};

class OV5642 : public ArduCam
{
public:
    typedef enum
    {
        BMP,
        JPEG,
        RAW,
    } ImgType;

    enum LightMode
    {
        ADVANCED_AWB,
        SIMPLE_AWB,
        MANUAL_DAY,
        MANUAL_A,
        MANUAL_CWF,
        MANUAL_CLOUDY,
    };

    // Saturation, Brightness, Contrast
    enum Calibration
    {
        CAL_PLUS_4,
        CAL_PLUS_3,
        CAL_PLUS_2,
        CAL_PLUS_1,
        CAL_ZERO,
        CAL_MINUS_1,
        CAL_MINUS_2,
        CAL_MINUS_3,
        CAL_MINUS_4,
    };

    enum Hue
    {
        HUE_PLUS_180,
        HUE_PLUS_150,
        HUE_PLUS_120,
        HUE_PLUS_90,
        HUE_PLUS_60,
        HUE_PLUS_30,
        HUE_0,
        HUE_MINUS_30,
        HUE_MINUS_60,
        HUE_MINUS_90,
        HUE_MINUS_120,
        HUE_MINUS_150,
    };

    enum JpegSize
    {
        DIMENSION_320x240,
        DIMENSION_640x480,
        DIMENSION_1024x768,
        DIMENSION_1280x960,
        DIMENSION_1600x1200,
        DIMENSION_2048x1536,
        DIMENSION_2592x1944,
    };

    enum SpecialEffects
    {
//            ANTIQUE,
        BLUEISH,
        GREENISH,
        REDDISH,
        BW,
        NEGATIVE,
//            BW_NEGATIVE,
        NORMAL,
        SEPIA,
    };

    enum Sharpness
    {
        AUTO_SHARPNESS,
        AUTO_SHARPNESS_1,
        AUTO_SHARPNESS_2,
        MANUAL_SHARPNESS_OFF,
        MANUAL_SHARPNESS_1,
        MANUAL_SHARPNESS_2,
        MANUAL_SHARPNESS_3,
        MANUAL_SHARPNESS_4,
        MANUAL_SHARPNESS_5,
    };

    enum MirrorFlip
    {
        MIRROR,
        FLIP,
        MIRROR_FLIP,
        MIRROR_NORMAL,
    };

    enum Compress
    {
        HIGH_QUALITY,
        DEFAULT_QUALITY,
        LOW_QUALITY,
    };

    enum TestPattern
    {
        COLOR_BAR,
        COLOR_SQUARE,
        BW_SQUARE,
        DLI,
    };

    enum ExposureLevel
    {
        MINUS_17_EV,
        MINUS_13_EV,
        MINUS_10_EV,
        MINUS_07_EV,
        MINUS_03_EV,
        EXPOSURE_DEFAULT,
        PLUS_03_EV,
        PLUS_07_EV,
        PLUS_10_EV,
        PLUS_13_EV,
        PLUS_17_EV,
    };

    explicit OV5642(GpioPin cs_pin_,
                    i2c_mux_t mux = I2C_MUX_CH_NONE,
                    ImgType format = JPEG);
    void init() override;

    void set_special_effects(SpecialEffects effect);
    void set_contrast(Calibration contrast);
    void set_brightness(Calibration brightness);
    void set_color_saturation(Calibration color_saturation);
    void set_light_mode(LightMode light_mode);
    void set_hue(Hue hue);
    void set_jpeg_size(JpegSize size);
    void set_sharpness(Sharpness sharpness);
    void set_mirror_flip(MirrorFlip mirror_flip);
    void set_compress_quality(Compress quality);
    void set_exposure_level(ExposureLevel level);

    void test_pattern(TestPattern pattern);

private:
    /**
     * The OV5642 can operate with different image format
     */
    ImgType m_fmt;
};

#define OV2640_MINI_2MP

//Define maximum frame buffer size
#if (defined OV2640_MINI_2MP)
#define MAX_FIFO_SIZE		0x5FFFF			//384KByte
#elif (defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined ARDUCAM_SHIELD_REVC)
#define MAX_FIFO_SIZE		0x7FFFF			//512KByte
#else
#define MAX_FIFO_SIZE        0x7FFFFF        //8MByte
#endif

/****************************************************/
/* ArduChip registers definition 											*/
/****************************************************/
#define RWBIT                                    0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1        0x00  //TEST register

#if !(defined OV2640_MINI_2MP)
#define ARDUCHIP_FRAMES              0x01  //FRAME control register, Bit[2:0] = Number of frames to be captured																		//On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#endif

#define ARDUCHIP_MODE            0x02  //Mode register
#define MCU2LCD_MODE            0x00
#define CAM2LCD_MODE            0x01
#define LCD2MCU_MODE            0x02

#define ARDUCHIP_TIM            0x03  //Timing control
#if !(defined OV2640_MINI_2MP)
#define HREF_LEVEL_MASK            0x01  //0 = High active , 		1 = Low active
#define VSYNC_LEVEL_MASK        0x02  //0 = High active , 		1 = Low active
#define LCD_BKEN_MASK            0x04  //0 = Enable, 					1 = Disable
#if (defined ARDUCAM_SHIELD_V2)
#define PCLK_REVERSE_MASK  	0x08  //0 = Normal PCLK, 		1 = REVERSED PCLK
#else
#define PCLK_DELAY_MASK        0x08  //0 = data no delay,		1 = data delayed one PCLK
#endif
//#define MODE_MASK          		0x10  //0 = LCD mode, 				1 = FIFO mode
#endif
//#define FIFO_PWRDN_MASK	   		0x20  	//0 = Normal operation, 1 = FIFO power down
//#define LOW_POWER_MODE			  0x40  	//0 = Normal mode, 			1 = Low power mode

#define ARDUCHIP_FIFO            0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK            0x01
#define FIFO_START_MASK            0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO              0x06  //GPIO Write Register
#if !(defined (ARDUCAM_SHIELD_V2) || defined (ARDUCAM_SHIELD_REVC))
#define GPIO_RESET_MASK            0x01  //0 = Sensor reset,							1 =  Sensor normal operation
#define GPIO_PWDN_MASK            0x02  //0 = Sensor normal operation, 	1 = Sensor standby
#define GPIO_PWREN_MASK            0x04    //0 = Sensor LDO disable, 			1 = sensor LDO enable
#endif

#define BURST_FIFO_READ            0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ        0x3D  //Single FIFO read operation

#define ARDUCHIP_REV            0x40  //ArduCHIP revision
#define VER_LOW_MASK            0x3F
#define VER_HIGH_MASK            0xC0

#define ARDUCHIP_TRIG            0x41  //Trigger source
#define VSYNC_MASK                0x01
#define SHUTTER_MASK            0x02
#define CAP_DONE_MASK            0x08

#define FIFO_SIZE1                0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2                0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3                0x44  //Camera write FIFO size[18:16]


#define WRITE_BIT 0x80

#endif //CMPE460_ARDU_CAM_H
