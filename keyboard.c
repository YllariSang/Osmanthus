#include <stdint.h>

extern void shell_input_char(char c);

/* Low-level port I/O wrappers matched strictly for scalar x86 compiler compilation */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

/* Comprehensive Translation Scan Matrix (Standard US-QWERTY Mapping Scheme) */
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

void init_keyboard(void) {
    // Controller initialization lines if required
}

/* Hardware Interrupt service routine bound directly to IRQ1 via your IDT table entry */
void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    
    // Issue the explicit End-Of-Interrupt token to the Master PIC command port
    outb(0x20, 0x20);

    // If the top bit of the byte is clear, the key was pressed down (Keypress Event)
    if (!(scancode & 0x80)) {
        char ascii = kbd_us[scancode];
        if (ascii != 0) {
            shell_input_char(ascii); // Send character directly into shell's latching buffer
        }
    }
}
