#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>

void     cursor_init(uint32_t screen_w, uint32_t screen_h);
void     cursor_move(int16_t dx, int16_t dy);
void     cursor_erase(void);
void     cursor_draw(void);
uint32_t cursor_x(void);
uint32_t cursor_y(void);

#endif