#include "keyboard.h"

/* Low-level inbound port byte reader */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

extern void shell_input_char(char c);

/* Tracker flag for modifier states */
static uint8_t shift_active = 0;

/* Standard Unshifted Scan Matrix */
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0,
   ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,  '-',   0,   0,   0,   0,  '+',   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0
};

/* Shifted Symbol Scan Matrix (Maps Dash to Underscore!) */
static const char kbd_us_shifted[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0,
   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',   0,
   ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,  '-',   0,   0,   0,   0,  '+',   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0
};

void init_keyboard(void) {
    /* Ready for input initialization signals can be placed here if needed */
}

void keyboard_handler(registers_t* regs __attribute__((unused))) {
    uint8_t scancode = inb(0x60);
    
    /* INTERCEPT MODIFIER KEY CODES */
    switch (scancode) {
        case 0x2A: /* Left Shift Pressed */
        case 0x36: /* Right Shift Pressed */
            shift_active = 1;
            return;
        case 0xAA: /* Left Shift Released */
        case 0xB6: /* Right Shift Released */
            shift_active = 0;
            return;
    }

    /* Process standard key strokes if it's a "Make" code (Press) */
    if (!(scancode & 0x80)) {
        /* Route execution to the correct translation table map matrix */
        char ascii = shift_active ? kbd_us_shifted[scancode] : kbd_us[scancode];
        
        if (ascii != 0) {
            shell_input_char(ascii);
        }
    }
}
