#pragma once
#include <stdint.h>

// raw syscall gate

static inline long syscall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    long ret;
    __asm__ volatile (
        "int $0x80"
        : "=a" (ret)
        : "a" (num), "S" (arg1), "d" (arg2), "c" (arg3)
        : "memory"
    );
    return ret;
}

// graphics

static inline uint64_t  plot_point(uint64_t x, uint64_t y, uint32_t color)
    { return syscall(0, x, y, color); }

static inline uint32_t *get_framebuffer(void)
    { return (uint32_t *)syscall(1, 0, 0, 0); }

static inline uint64_t  fb_info(uint64_t query)
    { return syscall(2, query, 0, 0); }

static inline uint64_t  get_fb_width(void)  { return fb_info(0); }
static inline uint64_t  get_fb_height(void) { return fb_info(1); }
static inline uint64_t  get_fb_pitch(void)  { return fb_info(2); }
static inline uint64_t  get_fb_bpp(void)    { return fb_info(3); }

// tty / debug

static inline int print(const char *msg, uint64_t len)
    { return (int)syscall(3, (uint64_t)msg, len, 0); }

static inline int tty_write(const char *msg, uint64_t len)
    { return (int)syscall(4, (uint64_t)msg, len, 0); }

// process

// fd: open ELF file descriptor, stack_size: 0 = default 8 MiB, parent: 0 = none
static inline int proc_create(uint64_t fd, uint64_t stack_size, uint64_t parent)
    { return (int)syscall(5, fd, stack_size, parent); }

static inline int proc_destroy(uint64_t pid)
    { return (int)syscall(6, pid, 0, 0); }

// threads

static inline int create_thread(uint64_t f_pointer, uint64_t stack_size)
    { return (int)syscall(7, f_pointer, stack_size, 0); }

// memory

static inline void *sbrk(long increment)
    { return (void *)syscall(8, (uint64_t)increment, 0, 0); }

// input

static inline int read_event(void *ev)
    { return (int)syscall(9, (uint64_t)ev, 0, 0); }

// file I/O

static inline int  open(const char *path, int flags, int mode)
    { return (int)syscall(10, (uint64_t)path, (uint64_t)flags, (uint64_t)mode); }

static inline int  close(int fd)
    { return (int)syscall(11, (uint64_t)fd, 0, 0); }

static inline long read(int fd, void *buf, uint64_t count)
    { return syscall(12, (uint64_t)fd, (uint64_t)buf, count); }

static inline long write(int fd, const void *buf, uint64_t count)
    { return syscall(13, (uint64_t)fd, (uint64_t)buf, count); }