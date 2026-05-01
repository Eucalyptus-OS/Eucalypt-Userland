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

context_t *context_new(uint32_t *buffer, uint32_t width,
                        uint32_t height, uint32_t pitch);

void context_fill_rect(context_t *ctx, uint32_t x, uint32_t y,
                       uint32_t width, uint32_t height, uint32_t color);

window_t *window_new(uint16_t x, uint16_t y, uint16_t width,
                       uint16_t height, context_t *ctx);
void window_paint(window_t *win);
void window_mark_dirty(window_t *win);
void window_free(window_t *win);