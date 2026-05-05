#pragma once
#include <stdint.h>

typedef struct context {
    uint32_t *buffer;
    uint32_t  width;
    uint32_t  height;
    uint32_t  pitch;
} context_t;

typedef struct window {
    uint16_t   x;
    uint16_t   y;
    uint16_t   width;
    uint16_t   height;
    bool       dirty;
    context_t *context;
} window_t;

int init_font_renderer();

void render_char(context_t *ctx, char c,
                     uint32_t x, uint32_t y,
                     uint32_t fg, uint32_t bg, int transparent_bg);

void render_text(context_t *ctx, const char *text,
                 uint32_t x, uint32_t y,
                 uint32_t fg, uint32_t bg, int transparent_bg);

uint32_t font_glyph_width();
uint32_t font_glyph_height();
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
context_t *context_new(uint32_t *buffer, uint32_t width,
                        uint32_t height, uint32_t pitch);
void context_fill_rect(context_t *ctx, uint32_t x, uint32_t y,
                       uint32_t width, uint32_t height, uint32_t color);
void context_put_pixel(context_t *ctx, uint32_t x, uint32_t y, uint32_t color);
window_t *window_new(uint16_t x, uint16_t y, uint16_t width,
                       uint16_t height, context_t *ctx);
void window_paint(window_t *win, uint32_t color);
void window_mark_dirty(window_t *win);
void window_free(window_t *win);