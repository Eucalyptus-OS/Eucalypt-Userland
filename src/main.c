#include <syscalls.h>
#include <display.h>
#include <compositor.h>
#include <vfs.h>
#include <stdio.h>
#include <term.h>
#include <event.h>

static char resolve_char(const input_event_t *ev) {
    uint8_t code = ev->code;
    int shift = ev->modifiers & (MOD_SHIFT | MOD_CAPS);

    if (code >= 'a' && code <= 'z')
        return shift ? (code - 32) : code;

    if (!shift) {
        if (code >= '0' && code <= '9') return code;
    } else {
        switch (code) {
            case '1': return '!'; case '2': return '@'; case '3': return '#';
            case '4': return '$'; case '5': return '%'; case '6': return '^';
            case '7': return '&'; case '8': return '*'; case '9': return '(';
            case '0': return ')'; case '-': return '_'; case '=': return '+';
            case '[': return '{'; case ']': return '}'; case '\\': return '|';
            case ';': return ':'; case '\'': return '"'; case ',': return '<';
            case '.': return '>'; case '/': return '?'; case '`': return '~';
        }
    }

    if (code == ' ' || code == '-' || code == '=' || code == '[' ||
        code == ']' || code == '\\' || code == ';' || code == '\'' ||
        code == ',' || code == '.' || code == '/' || code == '`')
        return (char)code;

    return 0;
}

void handle_event(const input_event_t *ev, context_t *ctx) {
    if (ev->kind != EVENT_KEY_PRESS) return;

    switch (ev->code) {
        case '\r':
        case '\n': {
            const char *cmd = submit_buf(ctx);
            handle_command(ctx, cmd);
            break;
        }
        case '\b':
        case 127:
            write_buf(ctx, '\b');
            break;
        default: {
            char c = resolve_char(ev);
            printf("%c\n", c);
            if (c) write_buf(ctx, c);
            break;
        }
    }
}

int main() {
    uint32_t *fb    = get_framebuffer();
    uint64_t width  = get_fb_width();
    uint64_t height = get_fb_height();
    uint64_t pitch  = get_fb_pitch();

    init_display();

    context_t *ctx = context_new(fb, width, height, pitch);
    if (!ctx) { printf("context_new failed\n"); while (1); }

    context_fill_rect(ctx, 0, 0, width, height, rgb(0, 0, 0, 255));

    int result = init_font_renderer();
    if (result < 0) {
        printf("init_font_renderer failed with code %d\n", result);
        while (1);
    }

    init_term(ctx);

    input_event_t ev;
    while (1) {
        if (poll_event(&ev) == 0)
            handle_event(&ev, ctx);
    }

    return 0;
}