#pragma once
#include <stdint.h>

static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;
    __asm__ volatile (
        "int $0x80"
        : "=a" (ret)
        : "a" (num), "S" (arg1), "d" (arg2), "c" (arg3)
        : "memory"
    );
    return ret;
}

static inline uint64_t  plot_point(uint64_t x, uint64_t y, uint32_t c) { return syscall(0, x, y, c); }
static inline uint32_t *get_framebuffer(void) { return (uint32_t *)syscall(1, 0, 0, 0); }
static inline uint64_t  fb_width(void)        { return syscall(2, 0, 0, 0); }
static inline uint64_t  fb_height(void)       { return syscall(2, 1, 0, 0); }
static inline uint64_t  fb_pitch(void)        { return syscall(2, 2, 0, 0); }
static inline void      tty_write(const char *msg, uint64_t len) { syscall(4, (uint64_t)msg, len, 0); }
// If parent is 0 the process will not have a parent
static inline uint8_t   proc_create(uint64_t node, uint64_t parent) { return syscall(5, node, parent, 0); }
static inline uint8_t   proc_destroy(uint64_t pid) { return syscall(6, pid, 0, 0); }
static inline void     *sbrk(long int increment) { return (void *)syscall(7, increment, 0, 0); }