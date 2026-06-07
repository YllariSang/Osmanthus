#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_writestring(const char* data);

void write_serial_char(char c);
void print_serial_string(const char* str);
char read_serial_char(void);

#endif
