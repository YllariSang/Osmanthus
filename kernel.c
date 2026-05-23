#include <stdint.h>
#include <stddef.h>

/* --- VGA TEXT MODE DRIVER CONSTANTS --- */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static uint16_t* const VGA_TEXT_BUFFER = (uint16_t*)0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

/* Forward declarations of core subsystem modules */
extern void init_gdt(void);
extern void init_idt(void);
extern void init_keyboard(void);
extern void shell_init(void);

/* Pull the standard boundary tracking marker symbol */
extern char _end[];
extern void kmalloc_init(uint32_t start_address, size_t initial_size);

/* --- VGA TEXT DRIVER ENGINE IMPLEMENTATION --- */
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(7, 0); // Light grey text on black background
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_TEXT_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
        return;
    }
    
    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            const size_t index = terminal_row * VGA_WIDTH + terminal_column;
            VGA_TEXT_BUFFER[index] = vga_entry(' ', terminal_color);
        }
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    VGA_TEXT_BUFFER[index] = vga_entry(c, terminal_color);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }
}

/* --- KERNEL INITIALIZATION ENTRY POINT --- */
void kernel_main(void) {
    /* CRITICAL INTERRUPT GUARD: Turn off hardware interrupts completely! */
    /* This stops background timer ticks from crashing the CPU during initialization */
    asm volatile("cli");

    /* Stabilize segment registers safely */
    init_gdt();

    /* Fire up the text console layout safely */
    terminal_initialize();
    terminal_writestring("Initializing Custom Arch Microkernel...\n");
    terminal_writestring("GDT Layout Initialization: SUCCESS\n");

    /* Bind our custom Interrupt Descriptor Table to claim CPU exception handling */
    init_idt();
    terminal_writestring("IDT Core Handlers Activated: SUCCESS\n");

    /* Hook the keyboard drivers */
    init_keyboard();
    terminal_writestring("Keyboard Driver Initialization: SUCCESS\n");

    /* Now that our custom IDT is active and secure, it is safe to turn interrupts back on! */
    asm volatile("sti");
    terminal_writestring("CPU Hardware Interrupt Lines Enabled: TRUE\n");

    /* Configure heap allocation space */
    uint32_t heap_placement_address = (uint32_t)_end;
    kmalloc_init(heap_placement_address, 1024 * 1024);

    shell_init();

    while (1) {
        asm volatile("hlt");
    }
}
