#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

typedef enum {
    OK = 0,
    NULL_POINTER,
    INVALID_DISPLAY_PARAMS,
    FAILED_TO_DRAW,
} fb_error;

fb_error init_display();

uint32_t color(uint8_t r, uint8_t g, uint8_t b);

fb_error draw_pixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t read_pixel(uint32_t x, uint32_t y);
fb_error draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
fb_error draw_rect_outline(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t thickness, uint32_t color);
fb_error draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
fb_error draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t color);
fb_error draw_circle_outline(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t color);
fb_error draw_triangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);

fb_error clear_screen(uint32_t color);
fb_error draw_bitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint32_t *bitmap);

uint32_t display_width(); 
uint32_t display_height();
uint32_t display_pitch();
uint32_t *display_buffer();

#endif