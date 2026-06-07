#include "graphics.h"

void graphics_enter_mode13(void) {
    graphics_clear_screen(0);
}

void graphics_clear_screen(uint8_t color) {
    volatile uint8_t* screen = (volatile uint8_t*)0xA0000;
    for (int i = 0; i < 320 * 200; i++) {
        screen[i] = color;
    }
}

void put_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < 320 && y >= 0 && y < 200) {
        volatile uint8_t* screen = (volatile uint8_t*)0xA0000;
        screen[y * 320 + x] = color;
    }
}

void draw_rectangle(int x, int y, int width, int height, uint8_t color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void draw_cursor(int x, int y) {
    draw_rectangle(x, y, 8, 8, 15); // Solid white square
}

/* External linkages and variables */
#include "stdio.h"
extern volatile char native_key_buffer; // Read directly from keyboard.c!

void run_graphics_demo(void) {
    int mouse_x = 160;
    int mouse_y = 100;
    uint8_t bg_color = 9; // Brilliant Canvas Blue
    
    graphics_clear_screen(bg_color);
    draw_rectangle(10, 10, 30, 30, 4); // Red Box
    draw_cursor(mouse_x, mouse_y);
    
    printf("\n=== NATIVE GRAPHICS DEMO ACTIVE ===\n");
    printf("Click your mouse inside the NOVM DISPLAY SCREEN window directly.\n");
    printf("Use [W, A, S, D] keys on your physical keyboard to glide the cursor block.\n");
    printf("Press [q] to quit.\n");

    // Force clear any leftover keystroke tokens before entering the loop
    native_key_buffer = '\0';

    while (1) {
        // Intercept asynchronous hardware keystroke events
        if (native_key_buffer != '\0') {
            char c = native_key_buffer;
            native_key_buffer = '\0'; // Clear the latch instantly
            
            // Wipe old tracking box position
            draw_rectangle(mouse_x, mouse_y, 8, 8, bg_color);
            
            // Handle coordinate changes natively
            if (c == 'w' || c == 'W') { if (mouse_y > 10)  mouse_y -= 8; }
            if (c == 's' || c == 'S') { if (mouse_y < 180) mouse_y += 8; }
            if (c == 'a' || c == 'A') { if (mouse_x > 10)  mouse_x -= 8; }
            if (c == 'd' || c == 'D') { if (mouse_x < 300) mouse_x += 8; }
            
            if (c == 'q' || c == 'Q') {
                printf("Exiting graphics canvas mode...\n");
                break;
            }
            
            // Maintain layout constraints
            draw_rectangle(10, 10, 30, 30, 4);
            
            // Re-render block destination
            draw_cursor(mouse_x, mouse_y);
        }
        
        // Let the CPU rest between interrupts
        asm volatile("hlt");
    }
}
