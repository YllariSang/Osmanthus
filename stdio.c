#include "stdio.h"
#include "string.h"
#include "kernel.h"

void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == 's') {
                char* s = va_arg(args, char*);
                terminal_writestring(s);
            } else if (*format == 'd') {
                int d = va_arg(args, int);
                char buf[32];
                itoa(d, buf, 10);
                terminal_writestring(buf);
            } else if (*format == 'x') {
                int x = va_arg(args, int);
                char buf[32];
                itoa(x, buf, 16);
                terminal_writestring(buf);
            } else if (*format == 'c') {
                char c = (char)va_arg(args, int);
                terminal_putchar(c);
            } else if (*format == '%') {
                terminal_putchar('%');
            }
        } else {
            terminal_putchar(*format);
        }
        format++;
    }

    va_end(args);
}
