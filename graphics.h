#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stddef.h>

#define VGA_SCREEN_WIDTH  320
#define VGA_SCREEN_HEIGHT 200
#define VGA_VIDEO_MEMORY  ((uint8_t*)0xA0000)

/* --- GRAPHICS ENGINE INTERFACE --- */
void graphics_enter_mode13(void);
void graphics_clear_screen(uint8_t color);
void put_pixel(int x, int y, uint8_t color);
void draw_rectangle(int x, int y, int width, int height, uint8_t color);

#endif
