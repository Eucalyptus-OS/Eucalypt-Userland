#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

void main(void) {
    uint32_t *fb    = get_framebuffer();
    uint64_t width  = fb_width();
    uint64_t height = fb_height();
    uint64_t pitch  = fb_pitch();
    uint64_t stride = pitch / 4;

    for (uint64_t y = 0; y < height; y++)
        for (uint64_t x = 0; x < width; x++)
            fb[y * stride + x] = 0xFF00FF;

    printf("%d\n", 1);
    while (1) {}
}