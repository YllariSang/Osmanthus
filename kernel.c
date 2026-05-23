#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "malloc.h"

/* --- VGA TERMINAL CONFIGURATION --- */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static volatile uint16_t* const VGA_BUFFER = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

extern void shell_init(void);
extern char end[]; /* Linker script symbol marking the end of binary data */

/* Low-level outbound port writer */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

/* Updates the hardware blinking cursor position via the VGA CRTC ports */
void update_cursor(void) {
    uint16_t position = (terminal_row * VGA_WIDTH) + terminal_column;

    /* Write the lower 8 bits of the cursor position offset (Register 15) */
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));

    /* Write the upper 8 bits of the cursor position offset (Register 14) */
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

/* --- TERMINAL ACTIONS --- */
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = 0x0F; /* White text on Black background */

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;
        }
    }
    update_cursor();
}

static void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t dest_index = (y - 1) * VGA_WIDTH + x;
            const size_t src_index = y * VGA_WIDTH + x;
            VGA_BUFFER[dest_index] = VGA_BUFFER[src_index];
        }
    }

    const size_t last_row_start = (VGA_HEIGHT - 1) * VGA_WIDTH;
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[last_row_start + x] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;
    }

    terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c) {
    /* BACKSPACE HANDLING GRAPHICS MATRIX */
    if (c == '\b') {
        if (terminal_column > 2) { /* Keeps it from deleting the shell token prompt '> ' */
            terminal_column--;
            const size_t index = terminal_row * VGA_WIDTH + terminal_column;
            VGA_BUFFER[index] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;
        }
        update_cursor();
        return;
    }

    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        update_cursor();
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    VGA_BUFFER[index] = (uint16_t) c | (uint16_t) terminal_color << 8;

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
    update_cursor();
}

void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }
}

/* --- KERNEL MAIN ENTRY POINT --- */
__attribute__((optimize("O0")))
void kernel_main(void) {
    terminal_initialize();
    terminal_writestring("Initializing Custom Arch Microkernel...\n");
    
    init_gdt();
    terminal_writestring("GDT Layout Initialization: SUCCESS\n");
    
    init_idt();
    terminal_writestring("IDT Core Handlers Activated: SUCCESS\n");

    init_keyboard();
    terminal_writestring("Keyboard Driver Initialization: SUCCESS\n");

    asm volatile("sti");
    terminal_writestring("CPU Hardware Interrupt Lines Enabled: TRUE\n");

    /* Initialize 1 MB of raw heap space right above our binary image location */
    uint32_t heap_placement_address = (uint32_t)end;
    kmalloc_init(heap_placement_address, 1024 * 1024);

    shell_init();
}
