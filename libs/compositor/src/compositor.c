#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <display.h>
#include <compositor.h>

context_t *context_new(uint32_t *buffer, uint32_t width,
                        uint32_t height, uint32_t pitch) {
    context_t *ctx = malloc(sizeof(*ctx));
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

uint8_t pseudo_rand_8(void) {
    static uint16_t seed = 1;
    seed = (uint16_t)(12657u * seed + 12345u);
    return (uint8_t)(seed >> 8);
}

void window_paint(window_t *win) {
    if (!win || !win->context) return;

    uint32_t color = ((uint32_t)0xFF           << 24) |  /* A */
                     ((uint32_t)pseudo_rand_8() << 16) |  /* B */
                     ((uint32_t)pseudo_rand_8() <<  8) |  /* G */
                      (uint32_t)pseudo_rand_8();           /* R */

    context_fill_rect(win->context, win->x, win->y,
                      win->width, win->height, color);
}

window_t *window_new(uint16_t x, uint16_t y,
                     uint16_t width, uint16_t height,
                     context_t *context) {
    window_t *win = malloc(sizeof(*win));
    if (!win) return NULL;

    win->x       = x;
    win->y       = y;
    win->width   = width;
    win->height  = height;
    win->context = context;
    win->dirty   = 1;

    return win;
}