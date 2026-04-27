#include <syscalls.h>
#include <display.h>
#include <window.h>

static wm_t wm;

void main() {
    init_display();
    wm_init(&wm);

    uint32_t sw = display_width();
    uint32_t sh = display_height();

    // file explorer style window
    window_t *explorer = wm_open(&wm, "File Explorer",
                                  sw / 2 - 300, sh / 2 - 200,
                                  600, 400,
                                  color(245, 245, 245));

    // settings window, slightly offset
    window_t *settings = wm_open(&wm, "Settings",
                                  sw / 2 - 100, sh / 2 - 250,
                                  500, 380,
                                  color(32, 32, 32));

    // a small dialog on top
    window_t *dialog = wm_open(&wm, "Are you sure?",
                                sw / 2 - 150, sh / 2 - 80,
                                300, 160,
                                color(40, 40, 40));

    // focus the dialog since it's on top
    wm_focus(&wm, dialog);

    wm_draw_all(&wm);

    while (1) {}
}