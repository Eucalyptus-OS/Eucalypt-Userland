#include <syscalls.h>
#include <stdint.h>
#include <display.h>

uint32_t fb_width  = 0;
uint32_t fb_height = 0;
uint32_t fb_pitch  = 0;
uint32_t *fb_ptr   = 0;

static inline void put_pixel(uint32_t x, uint32_t y, uint32_t c) {
    fb_ptr[y * (fb_pitch / 4) + x] = c;
}

static inline int abs_i(int v) { return v < 0 ? -v : v; }

fb_error init_display() {
    fb_pitch  = get_fb_pitch();
    fb_width  = get_fb_width();
    fb_height = get_fb_height();
    fb_ptr    = get_framebuffer();
    if (!fb_ptr)
        return NULL_POINTER;
    if (fb_pitch == 0 || fb_width == 0 || fb_height == 0)
        return INVALID_DISPLAY_PARAMS;
    return OK;
}

uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

fb_error draw_pixel(uint32_t x, uint32_t y, uint32_t c) {
    if (x >= fb_width || y >= fb_height)
        return FAILED_TO_DRAW;
    put_pixel(x, y, c);
    return OK;
}

fb_error draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t c) {
    if (x >= fb_width || y >= fb_height) return FAILED_TO_DRAW;
    uint32_t x_end = (x + width  > fb_width)  ? fb_width  : x + width;
    uint32_t y_end = (y + height > fb_height) ? fb_height : y + height;
    for (uint32_t row = y; row < y_end; row++)
        for (uint32_t col = x; col < x_end; col++)
            put_pixel(col, row, c);
    return OK;
}

fb_error draw_rect_outline(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                            uint32_t thickness, uint32_t c) {
    // top / bottom
    draw_rect(x, y,                    width,     thickness, c);
    draw_rect(x, y + height - thickness, width,   thickness, c);
    // left / right (inset to avoid overdrawing corners)
    draw_rect(x,                   y + thickness, thickness, height - thickness * 2, c);
    draw_rect(x + width - thickness, y + thickness, thickness, height - thickness * 2, c);
    return OK;
}

fb_error draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t c) {
    int dx  =  abs_i((int)x2 - (int)x1);
    int dy  = -abs_i((int)y2 - (int)y1);
    int sx  = x1 < x2 ? 1 : -1;
    int sy  = y1 < y2 ? 1 : -1;
    int err = dx + dy;

    int cx = (int)x1, cy = (int)y1;
    while (1) {
        if (cx >= 0 && cy >= 0 && (uint32_t)cx < fb_width && (uint32_t)cy < fb_height)
            put_pixel((uint32_t)cx, (uint32_t)cy, c);
        if (cx == (int)x2 && cy == (int)y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; cx += sx; }
        if (e2 <= dx) { err += dx; cy += sy; }
    }
    return OK;
}

fb_error draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t c) {
    int r = (int)radius;
    for (int y = -r; y <= r; y++)
        for (int x = -r; x <= r; x++)
            if (x*x + y*y <= r*r)
                draw_pixel((uint32_t)((int)cx + x), (uint32_t)((int)cy + y), c);
    return OK;
}

fb_error draw_circle_outline(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t c) {
    int x = 0, y = (int)radius, d = 1 - (int)radius;

    // plots 8 symmetric points
    #define PLOT8(px, py) do { \
        draw_pixel(cx+(px), cy+(py), c); draw_pixel(cx-(px), cy+(py), c); \
        draw_pixel(cx+(px), cy-(py), c); draw_pixel(cx-(px), cy-(py), c); \
        draw_pixel(cx+(py), cy+(px), c); draw_pixel(cx-(py), cy+(px), c); \
        draw_pixel(cx+(py), cy-(px), c); draw_pixel(cx-(py), cy-(px), c); \
    } while(0)

    while (x <= y) {
        PLOT8(x, y);
        if (d < 0) { d += 2*x + 3; }
        else       { d += 2*(x - y) + 5; y--; }
        x++;
    }
    #undef PLOT8
    return OK;
}

fb_error draw_triangle(uint32_t x0, uint32_t y0,
                        uint32_t x1, uint32_t y1,
                        uint32_t x2, uint32_t y2,
                        uint32_t c) {
    draw_line(x0, y0, x1, y1, c);
    draw_line(x1, y1, x2, y2, c);
    draw_line(x2, y2, x0, y0, c);
    return OK;
}

fb_error clear_screen(uint32_t c) {
    return draw_rect(0, 0, fb_width, fb_height, c);
}

fb_error draw_bitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                     const uint32_t *bitmap) {
    if (!bitmap) return NULL_POINTER;
    if (x >= fb_width || y >= fb_height) return FAILED_TO_DRAW;
    uint32_t x_end = (x + width  > fb_width)  ? fb_width  : x + width;
    uint32_t y_end = (y + height > fb_height) ? fb_height : y + height;
    for (uint32_t row = y; row < y_end; row++)
        for (uint32_t col = x; col < x_end; col++)
            put_pixel(col, row, bitmap[(row - y) * width + (col - x)]);
    return OK;
}

uint32_t display_width()  { return fb_width; }
uint32_t display_height() { return fb_height; }