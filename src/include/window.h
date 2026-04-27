#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <display.h>

#define MAX_WINDOWS 64
#define TITLE_BAR_HEIGHT 28
#define BORDER_SIZE 2
#define CLOSE_BTN_SIZE 16
#define WINDOW_TITLE_MAX 64

typedef struct window {
    struct window *next;
    struct window *prev;
    uint32_t id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t bg_color;
    uint8_t  visible;
    uint8_t  focused;
    char     title[WINDOW_TITLE_MAX];
} window_t;

typedef struct {
    window_t *head;
    window_t *tail;
    window_t *focused;
    uint32_t  next_id;
    uint32_t  count;
    window_t  pool[MAX_WINDOWS];
    uint8_t   pool_used[MAX_WINDOWS];
} wm_t;

// lifecycle
void      wm_init(wm_t *wm);
window_t *wm_open(wm_t *wm, const char *title, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t bg);
void      wm_close(wm_t *wm, window_t *w);

// state
void wm_focus(wm_t *wm, window_t *w);
void wm_hide(window_t *w);
void wm_show(window_t *w);
void wm_move(window_t *w, uint32_t x, uint32_t y);
void wm_resize(window_t *w, uint32_t width, uint32_t height);

// lookup
window_t *wm_find(wm_t *wm, uint32_t id);

// drawing
void wm_draw_all(wm_t *wm);

#endif