#include <oled.h>
#include <uart.h>
#include <string.h>

// There's two times more columns than rows
// Make sure that what I just said is not b.s.
COMPILE_ASSERT(OLED_WIDTH == OLED_HEIGHT * 2, oled_line_assert);

static void draw_line(OLEDCanvas canvas)
{
    // Clear everything
    memset(canvas, 0, sizeof(OLEDCanvas));

    // Draw a line from the top left to bottom right
    for (U32 i = 0; i < OLED_HEIGHT; i++)
    {
        U32 row = i / 8;  // 8 bits on each page
        U8 val = 1 << (i % 8);  // select the correct bit
        U32 col_1 = i * 2; // move to the correct column

        // Make sure we don't go out of bounds
        U32 idx_1 = row * OLED_WIDTH + col_1;
        U32 idx_2 = row * OLED_WIDTH + col_1 + 1;
        FW_ASSERT(idx_2 < OLED_SCREEN, row, col_1, idx_2, val);

        // Fill in both of the columns on this row
        canvas[idx_1] = val;
        canvas[idx_2] = val;
    }
}

int main()
{
    uart_init(UART_USB, 9600);
    oled_init();

    // Draw the first the part of the demo
    TextCanvas t = {.scroll = TRUE};
    oled_print(&t, "Hello");
    oled_print(&t, "line 2");
    oled_print(&t, "line 3");
    oled_print(&t, "line 4");
    oled_print(&t, "hello again");

    // Wait for input
    (void) uart_getchar(UART_USB);

    // Draw the second part of the demo
    OLEDCanvas line_canvas = {0};
    draw_line(line_canvas);
    oled_draw(line_canvas);
}
