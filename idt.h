#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* --- SYNCHRONIZED HARDWARE STACK FRAME MAP --- */
typedef struct registers {
    uint32_t ds;                                     /* Data segment selector pushed manually */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha execution */
    uint32_t error_code, int_no;                     /* Swapped to match assembly downward stack push growth order */
    uint32_t eip, cs, eflags, useresp, ss;           /* Pushed automatically by CPU on interrupt */
} registers_t;

typedef struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

/* --- EXPORT STRATEGIES --- */
void init_idt(void);

#endif
