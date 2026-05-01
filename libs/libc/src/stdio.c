#include <syscalls.h>
#include <string.h>
#include <stdarg.h>

// Convert an integer to a string
void itoa(int val, char *buf) {
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    int i = 0;
    int neg = val < 0;
    if (neg) val = -val;
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    if (neg) buf[i++] = '-';
    buf[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char tmp = buf[j];
        buf[j] = buf[k];
        buf[k] = tmp;
    }
}

// Print formatted output to the terminal
int printf(const char *str, ...) {
    va_list args;
    va_start(args, str);
    while (*str) {
        if (*str == '%') {
            str++;
            switch (*str) {
                case 'd': {
                    char buf[12];
                    itoa(va_arg(args, int), buf);
                    tty_write(buf, strlen(buf));
                    break;
                }
                case 's': {
                    const char *s = va_arg(args, const char *);
                    tty_write(s, strlen(s));
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    tty_write(&c, 1);
                    break;
                }
                case '%':
                    tty_write("%", 1);
                    break;
                case 'p': {
                    void *ptr = va_arg(args, void *);
                    char buf[16];
                    itoa(*(int *)ptr, buf);
                    tty_write(buf, strlen(buf));
                    break;
                }
                default:
                    tty_write("%", 1);
                    tty_write(str, 1);
                    break;
            }
        } else {
            tty_write(str, 1);
        }
        str++;
    }
    va_end(args);
    return 0;
}