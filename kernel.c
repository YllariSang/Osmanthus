#include <stdint.h>
#include <stddef.h>
#include "vga.h"
#include "timer.h"
#include "stdio.h"
#include "kernel.h"

#define SERIAL_PORT_COM1 0x3F8

/* Forward declarations of core subsystem modules */
extern void init_gdt(void);
extern void init_idt(void);
extern void init_keyboard(void);
extern void shell_init(void);
extern void pmm_init(uint32_t start_addr, uint32_t size);
extern void vmm_init(void);

extern char _end[];
extern void kmalloc_init(uint32_t start_address, size_t initial_size);

/* --- HARDWARE SERIAL I/O PORT UTILITIES (FIXED FOR X86 OPCODE ALIGNMENT) --- */

/* Forces an 8-bit byte value out to a 16-bit hardware port address */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

/* Reads an 8-bit byte value in from a 16-bit hardware port address */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Initializes the COM1 UART serial interface hardware controller */
void init_serial(void) {
    outb(SERIAL_PORT_COM1 + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_PORT_COM1 + 0, 0x03);    // Set divisor to 3 (38400 baud)
    outb(SERIAL_PORT_COM1 + 1, 0x00);    // High byte of divisor
    outb(SERIAL_PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

/* Returns true if the transmission buffer is clear to send data */
int is_transmit_empty(void) {
    return inb(SERIAL_PORT_COM1 + 5) & 0x20;
}

/* Sends a raw character out through the serial line */
void write_serial_char(char c) {
    while (is_transmit_empty() == 0);
    outb(SERIAL_PORT_COM1, c);
}

/* Streams a standard character string block to the serial interface */
void print_serial_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        write_serial_char(str[i]);
    }
}

/* Checks if a character is currently waiting to be pulled from the buffer */
int is_serial_received(void) {
    return inb(SERIAL_PORT_COM1 + 5) & 1;
}

/* Fetches a raw incoming character from the serial input data register */
char read_serial_char(void) {
    while (is_serial_received() == 0);
    return inb(SERIAL_PORT_COM1);
}

/* --- TERMINAL BACKEND COMPATIBILITY WRAPPERS FOR LINKER --- */
void terminal_initialize(void) {
    vga_init();
    print_serial_string("\n\r--- TERMINAL SCREEN RESET ---\n\r");
}

void terminal_putchar(char c) {
    vga_putchar(c);
    if (c == '\n') {
        write_serial_char('\n');
        write_serial_char('\r');
    } else {
        write_serial_char(c);
    }
}

void terminal_writestring(const char* data) {
    vga_writestring(data);
    print_serial_string(data);
}

/* --- KERNEL INITIALIZATION ENTRY POINT --- */
void kernel_main(void) {
    // Force clean state by disabling CPU interrupts during core hardware table setups
    asm volatile("cli");

    /* Fire up the text streaming serial connection */
    init_serial();
    
    /* Initialize VGA Text Console */
    vga_init();
    
    printf("=== OSMANTHUS MICROKERNEL ACTIVE ===\n");
    printf("Build Date: Jun 07 2026\n");

    init_gdt();
    printf("GDT Layout Initialization: SUCCESS\n");

    init_idt();
    printf("IDT Core Handlers Activated: SUCCESS\n");

    init_keyboard();
    printf("Keyboard Driver Initialization: SUCCESS\n");

    init_timer(100);
    printf("PIT Timer (100Hz) Initialization: SUCCESS\n");

    // Enable CPU hardware lines again now that tables are locked down safely
    asm volatile("sti");
    printf("CPU Hardware Interrupt Lines Enabled: TRUE\n");

    // Set up our kernel dynamic heap boundaries starting at the end of the loaded binary
    uint32_t heap_placement_address = (uint32_t)_end;
    kmalloc_init(heap_placement_address, 1024 * 1024); // Allocate 1MB Pool Area
    printf("Kernel Dynamic Heap System: READY (Base: 0x%x)\n", heap_placement_address);

    // Initialize Physical Memory Manager (PMM)
    uint32_t pmm_bitmap_addr = heap_placement_address + (1024 * 1024); // After 1MB heap
    pmm_init(pmm_bitmap_addr, 256 * 1024 * 1024); // Manage 256MB of physical memory
    printf("Physical Memory Manager: INITIALIZED (Bitmap: 0x%x)\n", pmm_bitmap_addr);

    // Initialize Virtual Memory Manager (VMM) - enables paging
    vmm_init();
    printf("Virtual Memory Manager: ENABLED (Paging Active)\n");

    printf("Dropping into system runtime interactive loop...\n");

    // Handoff execution cleanly to our automated serial interactive shell loop
    shell_init();

    // Fallback safety safety net loop
    while (1) {
        asm volatile("hlt");
    }
}
