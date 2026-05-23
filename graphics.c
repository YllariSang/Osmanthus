#include "graphics.h"

/* Low-level outbound I/O port writer helper */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

/* Low-level inbound port data state reader helper */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

/* Forces the CPU to wait for an I/O operation to complete */
static inline void io_wait(void) {
    outb(0x80, 0x00);
}

/* Hardcoded register array sequence to force standard VGA Mode 13h (320x200 256-color) */
static const uint8_t mode13_regs[] = {
/* MISC */          0x63,
/* SEQUENCE */      0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */          0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
                    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x9C, 0x8E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
/* GRAPHICS */      0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
/* ATTRIBUTE */     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x41, 0x00, 0x0F, 0x00, 0x00
};

void graphics_enter_mode13(void) {
    int i;
    const uint8_t* regs = mode13_regs;

    // 1. Write Miscellaneous Output Register
    outb(0x3C2, *regs++);
    io_wait();

    // 2. Write Sequencer Registers
    for (i = 0; i < 5; i++) {
        outb(0x3C4, i);
        outb(0x3C5, *regs++);
        io_wait();
    }

    // 3. Unlock CRTC registers (disable write protect on index 0-7)
    outb(0x3D4, 0x03); outb(0x3D5, inb(0x3D5) | 0x80);
    outb(0x3D4, 0x11); outb(0x3D5, inb(0x3D5) & ~0x80);
    io_wait();

    // 4. Write CRTC Controller Registers
    for (i = 0; i < 25; i++) {
        outb(0x3D4, i);
        outb(0x3D5, *regs++);
        io_wait();
    }

    // 5. Write Graphics Controller Registers
    for (i = 0; i < 9; i++) {
        outb(0x3CE, i);
        outb(0x3CF, *regs++);
        io_wait();
    }

    // 6. Write Attribute Controller Registers
    for (i = 0; i < 21; i++) {
        inb(0x3DA); // Reset Attribute flip-flop state
        outb(0x3C0, i);
        outb(0x3C0, *regs++);
        io_wait();
    }

    // Lock and activate video canvas output
    inb(0x3DA);
    outb(0x3C0, 0x20);

    // Clear everything to deep black right away
    graphics_clear_screen(0);
}

void graphics_clear_screen(uint8_t color) {
    for (int i = 0; i < VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT; i++) {
        VGA_VIDEO_MEMORY[i] = color;
    }
}

void put_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < VGA_SCREEN_WIDTH && y >= 0 && y < VGA_SCREEN_HEIGHT) {
        VGA_VIDEO_MEMORY[y * VGA_SCREEN_WIDTH + x] = color;
    }
}

void draw_rectangle(int x, int y, int width, int height, uint8_t color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}
