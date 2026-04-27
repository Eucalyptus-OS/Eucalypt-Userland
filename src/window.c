#include <window.h>
#include <display.h>
#include <stddef.h>
#include <stdint.h>

// ─── colors ──────────────────────────────────────────────────────────────────

#define COL_TASKBAR       color(32,  32,  32)
#define COL_DESKTOP       color(0,   120, 212)   // windows blue
#define COL_TITLEBAR_FOC  color(32,  32,  32)    // focused dark
#define COL_TITLEBAR_UNF  color(50,  50,  50)    // unfocused slightly lighter
#define COL_BORDER_FOC    color(0,   120, 212)   // blue accent border
#define COL_BORDER_UNF    color(70,  70,  70)
#define COL_CLOSE_HOVER   color(196, 43,  28)    // red
#define COL_CLOSE_NORMAL  color(60,  60,  60)
#define COL_TEXT          color(255, 255, 255)
#define COL_STARTBTN      color(0,   120, 212)

#define TASKBAR_H 48

// ─── string util (no libc) ───────────────────────────────────────────────────

static void str_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

// ─── pool alloc ──────────────────────────────────────────────────────────────

static window_t *pool_alloc(wm_t *wm) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!wm->pool_used[i]) {
            wm->pool_used[i] = 1;
            return &wm->pool[i];
        }
    }
    return NULL; // full
}

static void pool_free(wm_t *wm, window_t *w) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (&wm->pool[i] == w) {
            wm->pool_used[i] = 0;
            return;
        }
    }
}

// ─── init ────────────────────────────────────────────────────────────────────

void wm_init(wm_t *wm) {
    wm->head    = NULL;
    wm->tail    = NULL;
    wm->focused = NULL;
    wm->next_id = 1;
    wm->count   = 0;
    for (int i = 0; i < MAX_WINDOWS; i++)
        wm->pool_used[i] = 0;
}

// ─── open / close ────────────────────────────────────────────────────────────

window_t *wm_open(wm_t *wm, const char *title,
                  uint32_t x, uint32_t y,
                  uint32_t w, uint32_t h,
                  uint32_t bg) {
    window_t *win = pool_alloc(wm);
    if (!win) return NULL;

    win->id       = wm->next_id++;
    win->x        = x;
    win->y        = y;
    win->width    = w;
    win->height   = h;
    win->bg_color = bg;
    win->visible  = 1;
    win->focused  = 0;
    win->next     = NULL;
    win->prev     = wm->tail;
    str_copy(win->title, title, WINDOW_TITLE_MAX);

    if (wm->tail) wm->tail->next = win;
    else          wm->head       = win;
    wm->tail = win;
    wm->count++;

    wm_focus(wm, win);
    return win;
}

void wm_close(wm_t *wm, window_t *w) {
    if (wm->focused == w) wm->focused = NULL;

    if (w->prev) w->prev->next = w->next;
    else         wm->head      = w->next;
    if (w->next) w->next->prev = w->prev;
    else         wm->tail      = w->prev;

    wm->count--;
    pool_free(wm, w);
}

// ─── state ───────────────────────────────────────────────────────────────────

void wm_focus(wm_t *wm, window_t *w) {
    if (wm->focused) wm->focused->focused = 0;
    wm->focused  = w;
    w->focused   = 1;
}

void wm_hide(window_t *w) { w->visible = 0; }
void wm_show(window_t *w) { w->visible = 1; }

void wm_move(window_t *w, uint32_t x, uint32_t y) {
    w->x = x; w->y = y;
}

void wm_resize(window_t *w, uint32_t width, uint32_t height) {
    w->width = width; w->height = height;
}

window_t *wm_find(wm_t *wm, uint32_t id) {
    for (window_t *w = wm->head; w; w = w->next)
        if (w->id == id) return w;
    return NULL;
}

// ─── drawing ─────────────────────────────────────────────────────────────────

static void draw_char(uint32_t x, uint32_t y, char c, uint32_t col) {
    // stub — plug in your font renderer here
    (void)x; (void)y; (void)c; (void)col;
}

static void draw_string(uint32_t x, uint32_t y, const char *s, uint32_t col) {
    for (int i = 0; s[i]; i++)
        draw_char(x + i * 8, y, s[i], col);
}

static void draw_window(window_t *w) {
    if (!w->visible) return;

    uint32_t border = w->focused ? COL_BORDER_FOC : COL_BORDER_UNF;
    uint32_t tbar   = w->focused ? COL_TITLEBAR_FOC : COL_TITLEBAR_UNF;

    // outer border
    draw_rect_outline(w->x, w->y, w->width, w->height + TITLE_BAR_HEIGHT, BORDER_SIZE, border);

    // title bar
    draw_rect(w->x + BORDER_SIZE,
              w->y + BORDER_SIZE,
              w->width - BORDER_SIZE * 2,
              TITLE_BAR_HEIGHT,
              tbar);

    // title text
    draw_string(w->x + 12, w->y + 8, w->title, COL_TEXT);

    // window controls: minimize, maximize, close
    uint32_t btn_y  = w->y + BORDER_SIZE + 6;
    uint32_t btn_x  = w->x + w->width - BORDER_SIZE - 16;

    // close (red)
    draw_rect(btn_x,       btn_y, 12, 12, color(196, 43,  28));
    // maximize (grey)
    draw_rect(btn_x - 20,  btn_y, 12, 12, color(80,  80,  80));
    // minimize (grey)
    draw_rect(btn_x - 40,  btn_y, 12, 12, color(80,  80,  80));

    // client area
    draw_rect(w->x + BORDER_SIZE,
              w->y + BORDER_SIZE + TITLE_BAR_HEIGHT,
              w->width  - BORDER_SIZE * 2,
              w->height - BORDER_SIZE,
              w->bg_color);
}

static void draw_taskbar(wm_t *wm) {
    uint32_t sw = display_width();
    uint32_t sh = display_height();

    // taskbar background
    draw_rect(0, sh - TASKBAR_H, sw, TASKBAR_H, COL_TASKBAR);

    // start button
    draw_rect(sw / 2 - 20, sh - TASKBAR_H + 8, 40, 32, COL_STARTBTN);

    // taskbar entries — one pill per window
    uint32_t slot_x = 20;
    for (window_t *w = wm->head; w; w = w->next) {
        uint32_t pill = w->focused ? color(0, 120, 212) : color(60, 60, 60);
        draw_rect(slot_x, sh - TASKBAR_H + 10, 120, 28, pill);
        draw_string(slot_x + 8, sh - TASKBAR_H + 17, w->title, COL_TEXT);
        // active indicator bar
        if (w->focused)
            draw_rect(slot_x + 40, sh - 6, 40, 3, color(0, 120, 212));
        slot_x += 130;
    }
}

void wm_draw_all(wm_t *wm) {
    // desktop
    clear_screen(COL_DESKTOP);

    // windows back to front
    for (window_t *w = wm->head; w; w = w->next)
        draw_window(w);

    // taskbar on top of everything
    draw_taskbar(wm);
}