#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <display.h>
#include <compositor.h>

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((a << 24) | (b << 16) | (g << 8) | r);
}

context_t *context_new(uint32_t *buffer, uint32_t width,
                        uint32_t height, uint32_t pitch) {
    context_t *ctx = malloc(sizeof(struct context));
    if (!ctx) return NULL;
    ctx->buffer = buffer;
    ctx->width  = width;
    ctx->height = height;
    ctx->pitch  = pitch;
    return ctx;
}

void context_fill_rect(context_t *ctx, uint32_t x, uint32_t y,
                       uint32_t width, uint32_t height, uint32_t color) {
    if (!ctx || !ctx->buffer) return;
    if (x >= ctx->width || y >= ctx->height) return;

    uint32_t max_x = (width  <= ctx->width  - x) ? x + width  : ctx->width;
    uint32_t max_y = (height <= ctx->height - y) ? y + height : ctx->height;

    uint32_t stride = ctx->pitch / sizeof(uint32_t);

    for (uint32_t row = y; row < max_y; row++) {
        uint32_t *line = ctx->buffer + row * stride;
        for (uint32_t col = x; col < max_x; col++)
            line[col] = color;
    }
}

void context_put_pixel(context_t *ctx, uint32_t x, uint32_t y, uint32_t color) {
    if (!ctx || !ctx->buffer) return;
    if (x >= ctx->width || y >= ctx->height) return;

    uint32_t stride = ctx->pitch / sizeof(uint32_t);
    uint32_t *line = ctx->buffer + y * stride;
    line[x] = color;
}

void window_paint(window_t *win, uint32_t color) {
    if (!win || !win->context) return;

    context_fill_rect(win->context, win->x, win->y,
                      win->width, win->height, color);
}

window_t *window_new(uint16_t x, uint16_t y,
                     uint16_t width, uint16_t height,
                     context_t *context) {
    window_t *win = malloc(sizeof(struct window));
    if (!win) return NULL;

    win->x       = x;
    win->y       = y;
    win->width   = width;
    win->height  = height;
    win->context = context;
    win->dirty   = 1;

    return win;
}