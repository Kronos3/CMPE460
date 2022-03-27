#include <string.h>

#include <drv/i2c.h>
#include <lib/oled.h>

typedef enum
{
    SSD1306_BAUD_RATE = 400000,
    SSD1306_ADDR = 0x3C,

    SSD1306_MULTIPLEX_RATIO = 0xA8,

// registers
    SSD1306_CONTROL_REG = 0x00,  // Co = 0, DC = 0
    SSD1306_DATA_REG = 0x40,

// SSD1306 PARAMETERS
    SSD1306_LCDWIDTH = OLED_WIDTH,
    SSD1306_LCDHEIGHT = OLED_HEIGHT,
    SSD1306_MAXROWS = 7,
    SSD1306_MAXCONTRAST = 0xFF,

// command table
    SSD1306_SETCONTRAST = 0x81,
    SSD1306_DISPLAYALLON_RESUME = 0xA4,
    SSD1306_DISPLAYALLON = 0xA5,
    SSD1306_NORMALDISPLAY = 0xA6,
    SSD1306_INVERTDISPLAY = 0xA7,
    SSD1306_DISPLAYOFF = 0xAE,
    SSD1306_DISPLAYON = 0xAF,

// scrolling commands
    SSD1306_SCROLL_RIGHT = 0x26,
    SSD1306_SCROLL_LEFT = 0x27,
    SSD1306_SCROLL_VERT_RIGHT = 0x29,
    SSD1306_SCROLL_VERT_LEFT = 0x2A,
    SSD1306_SET_VERTICAL = 0xA3,
    SSD1306_SCROLL_DISABLE = 0x2E,

// address setting
    SSD1306_SETLOWCOLUMN = 0x00,
    SSD1306_SETHIGHCOLUMN = 0x10,
    SSD1306_MEMORYMODE = 0x20,
    SSD1306_COLUMNADDRESS = 0x21,
    SSD1306_COLUMNADDRESS_MSB = 0x00,
    SSD1306_COLUMNADDRESS_LSB = 0x7F,
    SSD1306_PAGEADDRESS = 0x22,
    SSD1306_PAGE_START_ADDRESS = 0xB0,

// hardware configuration
    SSD1306_SETSTARTLINE = 0x40,
    SSD1306_SEGREMAP = 0xA1,
    SSD1306_SETMULTIPLEX = 0xA8,
    SSD1306_COMSCANINC = 0xC0,
    SSD1306_COMSCANDEC = 0xC8,
    SSD1306_SETDISPLAYOFFSET = 0xD3,
    SSD1306_SETCOMPINS = 0xDA,

// timing and driving
    SSD1306_SETDISPLAYCLOCKDIV = 0xD5,
    SSD1306_SETPRECHARGE = 0xD9,
    SSD1306_SETVCOMDETECT = 0xDB,
    SSD1306_NOP = 0xE3,

// power supply configuration
    SSD1306_CHARGEPUMP = 0x8D,
    SSD1306_EXTERNALVCC = 0x10,
    SSD1306_INTERNALVCC = 0x14,
    SSD1306_SWITCHCAPVCC = 0x20,
} oled_cmd_t;

static void oled_send_command(U8 cmd)
{
    const U8 i2c_data[2] = {SSD1306_CONTROL_REG, cmd};
    i2c_write(i2c_data, 2);
}

void oled_draw(const OLEDCanvas data)
{
    U8 i2c_data[2] = {SSD1306_DATA_REG, 0};
    i2c_data[0] = SSD1306_DATA_REG;
    for (U32 i = 0; i < OLED_SCREEN; i++)
    {
        i2c_data[1] = data[i];
        i2c_write(i2c_data, 2);
    }
}

void oled_init(void)
{
    i2c_init(SSD1306_BAUD_RATE);
    i2c_set_address(SSD1306_ADDR);

    // init SSD1306 settings
    oled_send_command(SSD1306_DISPLAYOFF);  // Display off
    oled_send_command(SSD1306_MULTIPLEX_RATIO);  // Set Multiplex Ratio
    oled_send_command(0x3F);

    oled_send_command(SSD1306_SETDISPLAYOFFSET);  // Set Display Offset
    oled_send_command(0x00);  // no Offset

    oled_send_command(SSD1306_DATA_REG);  // Set display Start Line
    oled_send_command(SSD1306_SEGREMAP);  // Set Segment Re-Map
    oled_send_command(SSD1306_COMSCANDEC);  // Set COM Output Scan Direction

    oled_send_command(SSD1306_NORMALDISPLAY);
    oled_send_command(SSD1306_SETDISPLAYCLOCKDIV);
    oled_send_command(0x80);

    oled_send_command(SSD1306_SETCOMPINS);  // Set COM Pins Hardware Configuration
    oled_send_command(0x12);

    oled_send_command(SSD1306_SETCONTRAST);  // Set Contrast Control
    oled_send_command(0x7F);

    oled_send_command(SSD1306_DISPLAYALLON_RESUME);  // Set Entire Display On/Off

    oled_send_command(SSD1306_NORMALDISPLAY);  // Set Normal/Inverse Display

    oled_send_command(SSD1306_SETPRECHARGE);  // Set Pre-Charge Period
    oled_send_command(0xF1);  // Snternal

    oled_send_command(SSD1306_SETVCOMDETECT);  // Set VCOMH Deselect Level
    oled_send_command(0x40);

    oled_send_command(SSD1306_SETDISPLAYCLOCKDIV);  // Set Display Clock Divide Ratio\Oscilator Frequency
    oled_send_command(0x80);  // the suggested ratio 0x80

    oled_send_command(SSD1306_CHARGEPUMP);  // Set Charge Pump

    oled_send_command(SSD1306_INTERNALVCC);  // Vcc internal

    oled_send_command(SSD1306_SETLOWCOLUMN);  // Set Lower Column Start Address

    oled_send_command(SSD1306_SETHIGHCOLUMN);  // Set Higher Column Start Address

    oled_send_command(SSD1306_PAGE_START_ADDRESS);  // Set Page Start Address for Page Addressing Mode

    // 00 - Horizontal Addressing Mode
    // 01 - Vertical Addressing Mode
    // 02 - Page Addressing Mode
    oled_send_command(SSD1306_MEMORYMODE);  // Set Memory Addressing Mode
    oled_send_command(0x00);

    oled_send_command(SSD1306_COLUMNADDRESS);  // Set Column Address (only for horizontal or vertical mode)
    oled_send_command(SSD1306_COLUMNADDRESS_MSB);
    oled_send_command(SSD1306_COLUMNADDRESS_LSB);

    oled_send_command(SSD1306_PAGEADDRESS);  // Set Page Address
    oled_send_command(0x00);
    oled_send_command(0x07);

    oled_send_command(SSD1306_SCROLL_DISABLE);  // Deactivate Scroll
    oled_display_on();

    // Clear the display
    const OLEDCanvas c = {0};
    oled_draw(c);
    oled_display_on();
}

void oled_clear(OLEDCanvas canvas)
{
    memset(canvas, 0, sizeof(OLEDCanvas));
}

//unsigned char a[]={0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20};

void oled_ascii(OLEDCanvas canvas, I32 row, I32 col, char ascii)
{
    const static U8 ascii_mappings[95][16] = {
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//  0
            {0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x30, 0x00, 0x00, 0x00},//! 1
            {0x00, 0x10, 0x0C, 0x06, 0x10, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//" 2
            {0x40, 0xC0, 0x78, 0x40, 0xC0, 0x78, 0x40, 0x00, 0x04, 0x3F, 0x04, 0x04, 0x3F, 0x04, 0x04, 0x00},//# 3
            {0x00, 0x70, 0x88, 0xFC, 0x08, 0x30, 0x00, 0x00, 0x00, 0x18, 0x20, 0xFF, 0x21, 0x1E, 0x00, 0x00},//$ 4
            {0xF0, 0x08, 0xF0, 0x00, 0xE0, 0x18, 0x00, 0x00, 0x00, 0x21, 0x1C, 0x03, 0x1E, 0x21, 0x1E, 0x00},//% 5
            {0x00, 0xF0, 0x08, 0x88, 0x70, 0x00, 0x00, 0x00, 0x1E, 0x21, 0x23, 0x24, 0x19, 0x27, 0x21, 0x10},//& 6
            {0x10, 0x16, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//' 7
            {0x00, 0x00, 0x00, 0xE0, 0x18, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0x18, 0x20, 0x40, 0x00},//( 8
            {0x00, 0x02, 0x04, 0x18, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x18, 0x07, 0x00, 0x00, 0x00},//) 9
            {0x40, 0x40, 0x80, 0xF0, 0x80, 0x40, 0x40, 0x00, 0x02, 0x02, 0x01, 0x0F, 0x01, 0x02, 0x02, 0x00},//'*' 10
            {0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x01, 0x00},//+ 11
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xB0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00},//, 12
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},//- 13
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00},//. 14
            {0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x18, 0x04, 0x00, 0x60, 0x18, 0x06, 0x01, 0x00, 0x00, 0x00},/// 15
            {0x00, 0xE0, 0x10, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x00, 0x0F, 0x10, 0x20, 0x20, 0x10, 0x0F, 0x00},//0 16
            {0x00, 0x10, 0x10, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00},//1 17
            {0x00, 0x70, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x30, 0x28, 0x24, 0x22, 0x21, 0x30, 0x00},//2 18
            {0x00, 0x30, 0x08, 0x88, 0x88, 0x48, 0x30, 0x00, 0x00, 0x18, 0x20, 0x20, 0x20, 0x11, 0x0E, 0x00},//3 19
            {0x00, 0x00, 0xC0, 0x20, 0x10, 0xF8, 0x00, 0x00, 0x00, 0x07, 0x04, 0x24, 0x24, 0x3F, 0x24, 0x00},//4 20
            {0x00, 0xF8, 0x08, 0x88, 0x88, 0x08, 0x08, 0x00, 0x00, 0x19, 0x21, 0x20, 0x20, 0x11, 0x0E, 0x00},//5 21
            {0x00, 0xE0, 0x10, 0x88, 0x88, 0x18, 0x00, 0x00, 0x00, 0x0F, 0x11, 0x20, 0x20, 0x11, 0x0E, 0x00},//6 22
            {0x00, 0x38, 0x08, 0x08, 0xC8, 0x38, 0x08, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},//7 23
            {0x00, 0x70, 0x88, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x1C, 0x22, 0x21, 0x21, 0x22, 0x1C, 0x00},//8 24
            {0x00, 0xE0, 0x10, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x00, 0x00, 0x31, 0x22, 0x22, 0x11, 0x0F, 0x00},//9 25
            {0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00},//: 26
            {0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00},//; 27
            {0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00},//< 28
            {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00},//= 29
            {0x00, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00},//> 30
            {0x00, 0x70, 0x48, 0x08, 0x08, 0x08, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x30, 0x36, 0x01, 0x00, 0x00},//? 31
            {0xC0, 0x30, 0xC8, 0x28, 0xE8, 0x10, 0xE0, 0x00, 0x07, 0x18, 0x27, 0x24, 0x23, 0x14, 0x0B, 0x00},//@ 32
            {0x00, 0x00, 0xC0, 0x38, 0xE0, 0x00, 0x00, 0x00, 0x20, 0x3C, 0x23, 0x02, 0x02, 0x27, 0x38, 0x20},//A 33
            {0x08, 0xF8, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x20, 0x11, 0x0E, 0x00},//B 34
            {0xC0, 0x30, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x07, 0x18, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00},//C 35
            {0x08, 0xF8, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x00},//D 36
            {0x08, 0xF8, 0x88, 0x88, 0xE8, 0x08, 0x10, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x23, 0x20, 0x18, 0x00},//E 37
            {0x08, 0xF8, 0x88, 0x88, 0xE8, 0x08, 0x10, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x03, 0x00, 0x00, 0x00},//F 38
            {0xC0, 0x30, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x07, 0x18, 0x20, 0x20, 0x22, 0x1E, 0x02, 0x00},//G 39
            {0x08, 0xF8, 0x08, 0x00, 0x00, 0x08, 0xF8, 0x08, 0x20, 0x3F, 0x21, 0x01, 0x01, 0x21, 0x3F, 0x20},//H 40
            {0x00, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00},//I 41
            {0x00, 0x00, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x00, 0xC0, 0x80, 0x80, 0x80, 0x7F, 0x00, 0x00, 0x00},//J 42
            {0x08, 0xF8, 0x88, 0xC0, 0x28, 0x18, 0x08, 0x00, 0x20, 0x3F, 0x20, 0x01, 0x26, 0x38, 0x20, 0x00},//K 43
            {0x08, 0xF8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x30, 0x00},//L 44
            {0x08, 0xF8, 0xF8, 0x00, 0xF8, 0xF8, 0x08, 0x00, 0x20, 0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x20, 0x00},//M 45
            {0x08, 0xF8, 0x30, 0xC0, 0x00, 0x08, 0xF8, 0x08, 0x20, 0x3F, 0x20, 0x00, 0x07, 0x18, 0x3F, 0x00},//N 46
            {0xE0, 0x10, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x00},//O 47
            {0x08, 0xF8, 0x08, 0x08, 0x08, 0x08, 0xF0, 0x00, 0x20, 0x3F, 0x21, 0x01, 0x01, 0x01, 0x00, 0x00},//P 48
            {0xE0, 0x10, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x0F, 0x18, 0x24, 0x24, 0x38, 0x50, 0x4F, 0x00},//Q 49
            {0x08, 0xF8, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x03, 0x0C, 0x30, 0x20},//R 50
            {0x00, 0x70, 0x88, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x38, 0x20, 0x21, 0x21, 0x22, 0x1C, 0x00},//S 51
            {0x18, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x18, 0x00, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x00, 0x00},//T 52
            {0x08, 0xF8, 0x08, 0x00, 0x00, 0x08, 0xF8, 0x08, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x20, 0x1F, 0x00},//U 53
            {0x08, 0x78, 0x88, 0x00, 0x00, 0xC8, 0x38, 0x08, 0x00, 0x00, 0x07, 0x38, 0x0E, 0x01, 0x00, 0x00},//V 54
            {0xF8, 0x08, 0x00, 0xF8, 0x00, 0x08, 0xF8, 0x00, 0x03, 0x3C, 0x07, 0x00, 0x07, 0x3C, 0x03, 0x00},//W 55
            {0x08, 0x18, 0x68, 0x80, 0x80, 0x68, 0x18, 0x08, 0x20, 0x30, 0x2C, 0x03, 0x03, 0x2C, 0x30, 0x20},//X 56
            {0x08, 0x38, 0xC8, 0x00, 0xC8, 0x38, 0x08, 0x00, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x00, 0x00},//Y 57
            {0x10, 0x08, 0x08, 0x08, 0xC8, 0x38, 0x08, 0x00, 0x20, 0x38, 0x26, 0x21, 0x20, 0x20, 0x18, 0x00},//Z 58
            {0x00, 0x00, 0x00, 0xFE, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x40, 0x40, 0x40, 0x00},//[ 59
            {0x00, 0x0C, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x38, 0xC0, 0x00},//\ 60
            {0x00, 0x02, 0x02, 0x02, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x7F, 0x00, 0x00, 0x00},//] 61
            {0x00, 0x00, 0x04, 0x02, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//^ 62
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},//_ 63
            {0x00, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//` 64
            {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x19, 0x24, 0x22, 0x22, 0x22, 0x3F, 0x20},//a 65
            {0x08, 0xF8, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x11, 0x20, 0x20, 0x11, 0x0E, 0x00},//b 66
            {0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x0E, 0x11, 0x20, 0x20, 0x20, 0x11, 0x00},//c 67
            {0x00, 0x00, 0x00, 0x80, 0x80, 0x88, 0xF8, 0x00, 0x00, 0x0E, 0x11, 0x20, 0x20, 0x10, 0x3F, 0x20},//d 68
            {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x22, 0x22, 0x22, 0x22, 0x13, 0x00},//e 69
            {0x00, 0x80, 0x80, 0xF0, 0x88, 0x88, 0x88, 0x18, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00},//f 70
            {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x6B, 0x94, 0x94, 0x94, 0x93, 0x60, 0x00},//g 71
            {0x08, 0xF8, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x20, 0x3F, 0x21, 0x00, 0x00, 0x20, 0x3F, 0x20},//h 72
            {0x00, 0x80, 0x98, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00},//i 73
            {0x00, 0x00, 0x00, 0x80, 0x98, 0x98, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x80, 0x80, 0x7F, 0x00, 0x00},//j 74
            {0x08, 0xF8, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x20, 0x3F, 0x24, 0x02, 0x2D, 0x30, 0x20, 0x00},//k 75
            {0x00, 0x08, 0x08, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00},//l 76
            {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x3F, 0x20, 0x00, 0x3F},//m 77
            {0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x20, 0x3F, 0x21, 0x00, 0x00, 0x20, 0x3F, 0x20},//n 78
            {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x20, 0x1F, 0x00},//o 79
            {0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xA1, 0x20, 0x20, 0x11, 0x0E, 0x00},//p 80
            {0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x0E, 0x11, 0x20, 0x20, 0xA0, 0xFF, 0x80},//q 81
            {0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x20, 0x20, 0x3F, 0x21, 0x20, 0x00, 0x01, 0x00},//r 82
            {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x33, 0x24, 0x24, 0x24, 0x24, 0x19, 0x00},//s 83
            {0x00, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x00, 0x00},//t 84
            {0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x10, 0x3F, 0x20},//u 85
            {0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x01, 0x0E, 0x30, 0x08, 0x06, 0x01, 0x00},//v 86
            {0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x0F, 0x30, 0x0C, 0x03, 0x0C, 0x30, 0x0F, 0x00},//w 87
            {0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x20, 0x31, 0x2E, 0x0E, 0x31, 0x20, 0x00},//x 88
            {0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x81, 0x8E, 0x70, 0x18, 0x06, 0x01, 0x00},//y 89
            {0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x21, 0x30, 0x2C, 0x22, 0x21, 0x30, 0x00},//z 90
            {0x00, 0x00, 0x00, 0x00, 0x80, 0x7C, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x40, 0x40},//{ 91
            {0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00},//| 92
            {0x00, 0x02, 0x02, 0x7C, 0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00, 0x00},//} 93
            {0x00, 0x06, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//~ 94
    };

    U32 ascii_index = (ascii - ' ');  // correct ascii offset
    FW_ASSERT(ascii_index < sizeof(ascii_mappings) / sizeof(ascii_mappings[0]), ascii_index);

    // Copy each vertical portion of the character one at a time
    for (U32 i = 0; i < OLED_CHARACTER_HEIGHT; i++)
    {
        //                  offset to line              offset to column     offset to vertical portion of character
        memcpy(canvas + (row * (OLED_LINE_SIZE)) + (col * OLED_CHARACTER_WIDTH) + (i * OLED_WIDTH),
               // Grab the portion of the character we want
               ascii_mappings[ascii_index] + (i * OLED_CHARACTER_WIDTH),
               OLED_CHARACTER_WIDTH);
    }
}

void oled_line(OLEDCanvas canvas, I32 row, I32 col, const char* ascii_str)
{
    for (I32 i = 0; i < OLED_LINE_LENGTH; i++)
    {
        // Exit early if the string ends
        if (!ascii_str[i]) break;

        oled_ascii(canvas, row, col++, ascii_str[i]);
    }
}

void oled_display_off(void)
{
    oled_send_command(SSD1306_DISPLAYOFF);
}

void oled_display_on(void)
{
    oled_send_command(SSD1306_DISPLAYON);
}

void oled_print(TextCanvas* tcanvas, const char* str)
{
    if (tcanvas->current_line + 1 > OLED_LINE_MAX)
    {
        if (tcanvas->scroll)
        {
            tcanvas->current_line = OLED_LINE_MAX - 1;

            // Move all text up one line
            for (U32 i = 0; i < OLED_LINE_MAX - 1; i++)
            {
                memcpy(tcanvas->canvas + (OLED_LINE_SIZE * i),
                       tcanvas->canvas + (OLED_LINE_SIZE * (i + 1)),
                       OLED_LINE_SIZE); // move line i + 1 to line i
            }

            oled_line(tcanvas->canvas, tcanvas->current_line, 0, str);
        }
        else
        {
            // Overwrite the old first line
            tcanvas->current_line = 0;
            oled_line(tcanvas->canvas, tcanvas->current_line++, 0, str);
        }
    }
    else
    {
        oled_line(tcanvas->canvas, tcanvas->current_line++, 0, str);
    }

    oled_draw(tcanvas->canvas);
}

void oled_text_clear(TextCanvas* tcanvas)
{
    oled_clear(tcanvas->canvas);
    tcanvas->current_line = 0;
    oled_draw(tcanvas->canvas);
}

void oled_camera_to_oled(OLEDCanvas canvas, const U16* camera_in_array)
{
    oled_clear(canvas);

    for (I32 i = 0; i < SSD1306_LCDWIDTH; i++)
    {
        // set pixel based on camera_in_array.
        // 0 = top of screen, SCREEN_HEIGHT bottom of screen...
        I32 px_height = SSD1306_LCDHEIGHT - ((camera_in_array[i] * SSD1306_LCDHEIGHT) / (0x3FFF));
        canvas[i + (SSD1306_LCDWIDTH * (px_height / 8))] |= (1 << (px_height & 7));
    }
}

void oled_floating_to_oled(OLEDCanvas canvas, const F64* array, F64 offset, U32 n)
{
    oled_clear(canvas);

    for(I32 i = 0; i < SSD1306_LCDWIDTH; i++)
    {
        F64 i_f = (((F64) i) / SSD1306_LCDWIDTH) * n;
        // set pixel based on camera_in_array.
        // 0 = top of screen, SCREEN_HEIGHT bottom of screen...
        I32 px_height = SSD1306_LCDHEIGHT - (I32)((array[(U32)i_f] + offset) * SSD1306_LCDHEIGHT);
        px_height = FW_MAX(px_height, 0);
        px_height = FW_MIN(px_height, SSD1306_LCDHEIGHT - 1);
        canvas[i + (SSD1306_LCDWIDTH * (px_height / 8))] |= (1 << (px_height & 7));
    }
}
