#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

/* Core Mode 13h Layout Dimensions */
#define VGA_SCREEN_WIDTH  320
#define VGA_SCREEN_HEIGHT 200

/* Absolute Bare-Metal Frame Buffer Base Memory Offset Address */
#define VGA_VIDEO_MEMORY  ((volatile uint8_t*)0xA0000)

/* Subsystem Interface Prototypes */
void graphics_enter_mode13(void);
void graphics_clear_screen(uint8_t color);
void put_pixel(int x, int y, uint8_t color);
void draw_rectangle(int x, int y, int width, int height, uint8_t color);
void draw_cursor(int x, int y);
void run_graphics_demo(void);

#endif
