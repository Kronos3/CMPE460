#include <oled.h>

int main()
{
    oled_init();

    TextCanvas t = {.scroll = TRUE};

    oled_print(&t, "Hello");
    oled_print(&t, "line 2");
    oled_print(&t, "line 3");
    oled_print(&t, "line 4");
    oled_print(&t, "hello again");
}
