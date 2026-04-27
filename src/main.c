#include <syscalls.h>
#include <display.h>
#include <window.h>
#include <event.h>
#include <cursor.h>

static wm_t      wm;
static window_t *win_explorer;
static window_t *win_settings;
static window_t *win_dialog;

static void handle_key(uint8_t code) {
    switch (code) {
    case (uint8_t)'1':
        if (win_dialog)   { wm_close(&wm, win_dialog);   win_dialog   = (window_t *)0; }
        break;
    case (uint8_t)'2':
        if (win_settings) { wm_close(&wm, win_settings); win_settings = (window_t *)0; }
        break;
    case (uint8_t)'3':
        if (win_explorer) { wm_close(&wm, win_explorer); win_explorer = (window_t *)0; }
        break;
    default:
        return;
    }
    if (wm.tail) wm_focus(&wm, wm.tail);
    cursor_erase();
    wm_draw_all(&wm);
    cursor_draw();
}

void main(void) {
    init_display();
    wm_init(&wm);

    uint32_t sw = display_width();
    uint32_t sh = display_height();

    cursor_init(sw, sh);

    win_explorer = wm_open(&wm, "File Explorer  [3]",
                            sw/2-300, sh/2-200, 600, 400, color(245,245,245));
    win_settings = wm_open(&wm, "Settings  [2]",
                            sw/2-100, sh/2-250, 500, 380, color(32,32,32));
    win_dialog   = wm_open(&wm, "Are you sure?  [1]",
                            sw/2-150, sh/2-80,  300, 160, color(40,40,40));

    wm_focus(&wm, win_dialog);
    wm_draw_all(&wm);
    cursor_draw();
    
    input_event_t ev;
    while (1) {
        if (!poll_event(&ev)) continue;

        if (ev.kind == EVENT_MOUSE_MOVE) {
            cursor_erase();
            cursor_move(ev.x, ev.y);
            cursor_draw();
        } else if (ev.kind == EVENT_KEY_PRESS) {
            handle_key(ev.code);
        }
    }
}