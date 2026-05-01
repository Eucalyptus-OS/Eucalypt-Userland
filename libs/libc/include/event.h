#pragma once
#include <stdint.h>

#define EVENT_NONE        0
#define EVENT_KEY_PRESS   1
#define EVENT_KEY_RELEASE 2
#define EVENT_MOUSE_MOVE  3
#define EVENT_MOUSE_BTN   4

#define MOD_SHIFT  (1u << 0)
#define MOD_CAPS   (1u << 1)
#define MOD_CTRL   (1u << 2)
#define MOD_ALT    (1u << 3)

typedef struct {
    uint8_t  kind;
    uint8_t  code;
    uint8_t  scancode;
    uint8_t  modifiers;
    int16_t  x;
    int16_t  y;
    uint8_t  _pad[8];
} input_event_t;

static inline int poll_event(input_event_t *ev) {
    uint64_t ret;
    __asm__ volatile (
        "int $0x80"
        : "=a" (ret)
        : "a"  ((uint64_t)8),
          "S"  ((uint64_t)(uintptr_t)ev),
          "d"  ((uint64_t)0),
          "c"  ((uint64_t)0)
        : "memory"
    );
    return (int)ret;
}
