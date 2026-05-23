#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"

/* --- VGA TERMINAL CONFIGURATION --- */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static volatile uint16_t* const VGA_BUFFER = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

extern void shell_init(void);

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
        return;
    }

    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
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
}

void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }
}

/* --- KERNEL ENTRY ENTRY INTERFACE --- */
__attribute__((optimize("O0")))
void kernel_main(void) {
    /* Initialize display terminal maps */
    terminal_initialize();
    terminal_writestring("Initializing Custom Arch Microkernel...\n");
    
    /* Hardware Initialization Sequence */
    init_gdt();
    terminal_writestring("GDT Layout Initialization: SUCCESS\n");
    
    init_idt();
    terminal_writestring("IDT Core Handlers Activated: SUCCESS\n");

    init_keyboard();
    terminal_writestring("Keyboard Driver Initialization: SUCCESS\n");

    /* Safely release CPU hardware gate line constraints */
    asm volatile("sti");
    terminal_writestring("CPU Hardware Interrupt Lines Enabled: TRUE\n");

    /* Hand off total runtime execution control to our Shell interface engine */
    shell_init();
}
