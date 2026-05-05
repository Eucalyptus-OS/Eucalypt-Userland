#include <syscalls.h>
#include <compositor.h>
#include <string.h>
#include <vfs.h>

#define TERM_COLS 80
#define TERM_ROWS 24
#define CHAR_W    8
#define CHAR_H    16
#define ORIGIN_X  10
#define ORIGIN_Y  10

char screen[TERM_ROWS][TERM_COLS];
int cur_col = 1;
int cur_row = 0;
char buf[TERM_COLS] = {0};
int buf_pos = 0;

static int col_to_x(int col) { return ORIGIN_X + col * CHAR_W; }
static int row_to_y(int row) { return ORIGIN_Y + row * CHAR_H; }

void init_term(context_t *ctx) {
    memset(screen, 0, sizeof(screen));
    screen[cur_row][0] = '>';
    render_text(ctx, ">", ORIGIN_X, ORIGIN_Y,
                rgb(255, 255, 255, 255), rgb(0, 0, 0, 255), 0);
}

void write_buf(context_t *ctx, char c) {
    if (c == '\b') {
        if (buf_pos > 0) {
            buf[--buf_pos] = '\0';
            if (cur_col > 1) cur_col--;
            screen[cur_row][cur_col] = '\0';
            char tmp[2] = {' ', '\0'};
            render_text(ctx, tmp,
                        col_to_x(cur_col), row_to_y(cur_row),
                        rgb(255, 255, 255, 255), rgb(0, 0, 0, 255), 0);
        }
        return;
    }
    if (buf_pos >= TERM_COLS - 2) return;
    buf[buf_pos++] = c;
    buf[buf_pos]   = '\0';
    screen[cur_row][cur_col] = c;
    char tmp[2] = {c, '\0'};
    render_text(ctx, tmp,
                col_to_x(cur_col), row_to_y(cur_row),
                rgb(255, 255, 255, 255), rgb(0, 0, 0, 255), 0);
    if (cur_col < TERM_COLS - 1) cur_col++;
}

const char *submit_buf(context_t *ctx) {
    static char cmd[TERM_COLS];
    memcpy(cmd, buf, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    buf_pos = 0;
    cur_row++;
    if (cur_row >= TERM_ROWS) cur_row = TERM_ROWS - 1;
    cur_col = 1;
    screen[cur_row][0] = '>';
    render_text(ctx, ">", col_to_x(0), row_to_y(cur_row),
                rgb(255, 255, 255, 255), rgb(0, 0, 0, 255), 0);
    return cmd;
}

int handle_command(context_t *ctx, const char *cmd) {
    if (strcmp(cmd, "clear") == 0) {
        memset(screen, 0, sizeof(screen));
        cur_row = 0;
        cur_col = 1;
        screen[cur_row][0] = '>';
        render_text(ctx, ">", col_to_x(0), row_to_y(cur_row),
                    rgb(255, 255, 255, 255), rgb(0, 0, 0, 255), 0);
        return 1;
    } else {
        int fd = open(cmd, O_RDONLY, 0);
        if (fd != -1) {
            proc_create(fd, 0x800000, 0);
            close(fd);
            return 1;
        }
    }
    return 0;
}