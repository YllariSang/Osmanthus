#include "timer.h"

static uint32_t tick = 0;

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

void init_timer(uint32_t frequency) {
    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency.
    uint32_t divisor = 1193180 / frequency;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

void timer_handler(registers_t* regs) {
    (void)regs;
    tick++;
}

uint32_t get_tick_count(void) {
    return tick;
}
