#include <display.h>
#include <compositor.h>

int main(void) {
    init_display();

    context_t *ctx = context_new(
        display_buffer(),
        display_width(),
        display_height(),
        display_pitch()
    );

    window_t *win1 = window_new(10, 10, 300, 200, ctx);
    window_paint(win1);

    while (1);

    return 0;
}