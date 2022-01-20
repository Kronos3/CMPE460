#ifndef CMPE460_OLED_H
#define CMPE460_OLED_H

#include <fw.h>

#ifndef __oled_LINKED__
#error "You need to link 'oled' to use this header"
#endif


enum {
    OLED_WIDTH = 128,
    OLED_HEIGHT = 64,
    OLED_LINE_LENGTH = 16,      // Maximum number of printable characters in one line
    OLED_LINE_MAX = 4,          // Maximum number of displayable lines
    OLED_CHARACTER_WIDTH = 8,   // Width of character in bytes
    OLED_CHARACTER_HEIGHT = 2,  // Height of character in bytes
    OLED_LINE_SIZE = OLED_WIDTH * OLED_CHARACTER_HEIGHT,       // Number of bytes per line
    OLED_SCREEN = (OLED_WIDTH * OLED_HEIGHT) / 8
};

// Make sure our parameters make sense
COMPILE_ASSERT(OLED_CHARACTER_WIDTH * OLED_LINE_LENGTH == OLED_WIDTH, oled_char_width);
COMPILE_ASSERT(OLED_LINE_SIZE == OLED_CHARACTER_HEIGHT * OLED_WIDTH, oled_char_height);
COMPILE_ASSERT(OLED_LINE_SIZE == OLED_LINE_LENGTH * OLED_CHARACTER_WIDTH * OLED_CHARACTER_HEIGHT, oled_char_width_length);
COMPILE_ASSERT(OLED_LINE_MAX == OLED_SCREEN / OLED_LINE_SIZE, oled_max_lines);

typedef U8 OLEDCanvas[OLED_SCREEN];
typedef struct {
    OLEDCanvas canvas;
    I32 current_line;       // Keep track of where to print the next line

    // Options
    bool_t scroll;          // If TRUE, text will be scrolled like a terminal, otherwise overwritten
} TextCanvas;

#define oled_clear(canvas) memset((canvas), 0, sizeof(OLEDCanvas))

/**
 * Initialize the OLED I2C device
 */
void oled_init(void);

/**
 * Convert a camera array canvas into an OLED canvas
 * Draw camera data on the OLED display
 * @param canvas OLED canvas to write to
 * @param camera_in_array camera data canvas
 */
void oled_camera_to_oled(OLEDCanvas canvas, const U16* camera_in_array);

/**
 * Draw an OLED canvas to the device via I2C
 * @param data canvas to draw
 */
void oled_draw(const OLEDCanvas data);

/**
 * Place an ASCII character into an OLED canvas at a certain coordinate
 * @param canvas canvas to place character in
 * @param row row in text display lattice
 * @param col column in text display lattice
 * @param ascii character to place
 */
void oled_ascii(OLEDCanvas canvas, I32 row, I32 col, char ascii);

/**
 * Draw an entire line to the screen at a certain coordinate
 * supports a 4 line display
 * @param canvas canvas to place line on
 * @param row row to write line on
 * @param col column to start line on
 * @param ascii_str line to display
 */
void oled_line(OLEDCanvas canvas, I32 row, I32 col, const char* ascii_str);

/**
 * Turn the entire display ON
 */
void oled_display_on(void);

/**
 * Turn the entire display OFF
 */
void oled_display_off(void);

/**
 * Clear the entire OLED display
 */
void oled_display_clear(void);

/**
 * Print a single line to a text canvas and draw the
 * underlying OLED canvas
 * @param tcanvas text canvas to print line to
 * @param str line to print
 */
void oled_print(TextCanvas* tcanvas, const char* str);

#endif //CMPE460_OLED_H
