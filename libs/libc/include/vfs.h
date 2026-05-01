#pragma once

#include "syscalls.h"

#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR   0x0002
#define O_CREAT  0x0040
#define O_TRUNC  0x0200
#define O_APPEND 0x0400
#define O_EXCL   0x0800

static inline int vfs_open(const char *path, int flags, int mode) {
    return open(path, flags, mode);
}

static inline int vfs_close(int fd) {
    return close(fd);
}