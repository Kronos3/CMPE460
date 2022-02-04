#include <oled.h>
#include <uart.h>
#include <string.h>

static void draw_line(OLEDCanvas canvas)
{
    // Clear everything
    memset(canvas, 0, sizeof(OLEDCanvas));

    for (U32 i = 0; i < OLED_HEIGHT; i++)
    {
        U32 row = i / 8;
        U8 val = 1 << (i % 8);
        U32 col_1 = i * 2;
        U32 col_2 = i * 2 + 1;

        canvas[row * OLED_WIDTH + col_1] = val;
        canvas[row * OLED_WIDTH + col_2] = val;
    }
}

int main()
{
    uart_init(UART_USB, 9600);
    oled_init();

    OLEDCanvas line_canvas = {0};
    draw_line(line_canvas);
    oled_draw(line_canvas);

    // Wait for input
    (void) uart_getchar(UART_USB);

    TextCanvas t = {.scroll = TRUE};

    oled_print(&t, "Hello");
    oled_print(&t, "line 2");
    oled_print(&t, "line 3");
    oled_print(&t, "line 4");
    oled_print(&t, "hello again");
}
