#include <cursor.h>
#include <display.h>

#define CURSOR_WI 12
#define CURSOR_HE 19

static const uint8_t SHAPE[CURSOR_HE][CURSOR_WI] = {
    {1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0},
    {1,2,2,2,2,2,1,1,1,1,0,0},
    {1,2,2,2,1,2,2,1,0,0,0,0},
    {1,2,2,1,0,1,2,2,1,0,0,0},
    {1,2,1,0,0,0,1,2,2,1,0,0},
    {1,1,0,0,0,0,0,1,2,2,1,0},
    {1,0,0,0,0,0,0,0,1,2,2,1},
    {0,0,0,0,0,0,0,0,0,1,2,1},
    {0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,1},
};

static int32_t  g_cx, g_cy;
static uint32_t g_sw, g_sh;
static uint32_t g_bg[CURSOR_HE * CURSOR_WI];
static int32_t  g_bg_x, g_bg_y;
static uint8_t  g_bg_valid;

void cursor_init(uint32_t sw, uint32_t sh) {
    g_sw = sw;
    g_sh = sh;
    g_cx = (int32_t)(sw / 2);
    g_cy = (int32_t)(sh / 2);
    g_bg_valid = 0;
}

void cursor_move(int16_t dx, int16_t dy) {
    g_cx += dx;
    g_cy += dy;
    if (g_cx < 0)               g_cx = 0;
    if (g_cy < 0)               g_cy = 0;
    if (g_cx >= (int32_t)g_sw)  g_cx = (int32_t)g_sw - 1;
    if (g_cy >= (int32_t)g_sh)  g_cy = (int32_t)g_sh - 1;
}

void cursor_erase(void) {
    if (!g_bg_valid) return;
    for (int row = 0; row < CURSOR_HE; row++) {
        for (int col = 0; col < CURSOR_WI; col++) {
            uint32_t sx = (uint32_t)(g_bg_x + col);
            uint32_t sy = (uint32_t)(g_bg_y + row);
            draw_pixel(sx, sy, g_bg[row * CURSOR_WI + col]);
        }
    }
    g_bg_valid = 0;
}

void cursor_draw(void) {
    for (int row = 0; row < CURSOR_HE; row++) {
        for (int col = 0; col < CURSOR_WI; col++) {
            uint32_t sx = (uint32_t)(g_cx + col);
            uint32_t sy = (uint32_t)(g_cy + row);
            g_bg[row * CURSOR_WI + col] = read_pixel(sx, sy);
        }
    }
    g_bg_x = g_cx;
    g_bg_y = g_cy;
    g_bg_valid = 1;

    for (int row = 0; row < CURSOR_HE; row++) {
        for (int col = 0; col < CURSOR_WI; col++) {
            uint8_t px = SHAPE[row][col];
            if (!px) continue;
            uint32_t sx = (uint32_t)(g_cx + col);
            uint32_t sy = (uint32_t)(g_cy + row);
            uint32_t c  = (px == 1) ? color(0, 0, 0) : color(255, 255, 255);
            draw_pixel(sx, sy, c);
        }
    }
}

uint32_t cursor_x(void) { return (uint32_t)g_cx; }
uint32_t cursor_y(void) { return (uint32_t)g_cy; }