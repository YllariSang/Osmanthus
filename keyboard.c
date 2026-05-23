#include "keyboard.h"

/* --- US KEYBOARD SCANCODE MAPPING MATRIX --- */
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ', 0
};

extern void terminal_putchar(char c);

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void init_keyboard(void) {
    while (inb(0x64) & 1) {
        inb(0x60);
    }
}

/* Force global symbol mapping visibility flag */
void keyboard_handler(registers_t* regs __attribute__((unused))) {
    uint8_t scancode = inb(0x60);
    
    // Process when a key is pressed down (bit 7 clear)
    if (!(scancode & 0x80)) {
        char ascii = kbd_us[scancode];
        if (ascii != 0) {
            terminal_putchar(ascii);
        }
    }
}
