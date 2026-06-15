#include "idt.h"
#include "keyboard.h"
#include "timer.h"
#include <stddef.h>

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

extern void idt_load(uint32_t);
extern void isr0();  extern void isr1();  extern void isr2();  extern void isr3();
extern void isr4();  extern void isr5();  extern void isr6();  extern void isr7();
extern void isr8();  extern void isr9();  extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();

extern void irq0();  extern void irq1();  extern void irq2();  extern void irq3();
extern void irq4();  extern void irq5();  extern void irq6();  extern void irq7();
extern void irq8();  extern void irq9();  extern void irq10(); extern void irq11();
extern void irq12(); extern void irq13(); extern void irq14(); extern void irq15();

extern void terminal_writestring(const char* data);
extern void terminal_putchar(char c);

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline void io_wait(void) {
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low  = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].sel       = sel;
    idt_entries[num].always0   = 0;
    idt_entries[num].flags     = flags;
}

void init_idt(void) {
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    outb(0x20, 0x11); io_wait();
    outb(0xA0, 0x11); io_wait();
    outb(0x21, 0x20); io_wait(); 
    outb(0xA1, 0x28); io_wait(); 
    outb(0x21, 0x04); io_wait();
    outb(0xA1, 0x02); io_wait();
    outb(0x21, 0x01); io_wait();
    outb(0xA1, 0x01); io_wait();

    /* HARDWARE DEADLOCK RESOLUTION FIXED MASK:
       Unmask Timer (IRQ0) AND Keyboard (IRQ1) -> 11111100b = 0xFC */
    outb(0x21, 0xFC); 
    outb(0xA1, 0xFF); 

    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E); idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E); idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E); idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E); idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E); idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E); idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E); idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E); idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E); idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E); idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E); idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E); idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E); idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E); idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E); idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E); idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E); idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E); idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E); idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E); idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E); idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E); idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E); idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E); idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    idt_load((uint32_t)&idt_ptr);
}

void isr_handler(registers_t regs) {
    uint32_t int_no = regs.int_no;
    
    // Exception names for better diagnostics
    const char* exception_names[] = {
        "Divide by Zero", "Debug", "NMI", "Breakpoint",
        "Overflow", "Bound Range", "Invalid Opcode", "Device Not Available",
        "Double Fault", "Coprocessor Segment", "Invalid TSS", "Segment Not Present",
        "Stack Segment", "General Protection", "Page Fault", "Reserved",
        "Floating Point", "Alignment Check", "Machine Check", "SIMD Exception"
    };

    terminal_writestring("\n========== EXCEPTION OCCURRED ==========\n");
    terminal_writestring("Exception ID: ");
    
    if (int_no < 20) {
        terminal_writestring(exception_names[int_no]);
    } else {
        char buf[12];
        int i = 10;
        buf[11] = '\0';
        uint32_t n = int_no;
        while (n > 0 && i >= 0) {
            buf[i--] = (n % 10) + '0';
            n /= 10;
        }
        terminal_writestring(&buf[i + 1]);
    }
    terminal_writestring(" (");
    char buf[12];
    int i = 10;
    buf[11] = '\0';
    uint32_t n = int_no;
    while (n > 0 && i >= 0) {
        buf[i--] = (n % 10) + '0';
        n /= 10;
    }
    terminal_writestring(&buf[i + 1]);
    terminal_writestring(")\n");
    
    // Print error code if present
    if (int_no == 8 || (int_no >= 10 && int_no <= 14)) {
        terminal_writestring("Error Code: 0x");
        char hex_buf[16];
        int j = 0;
        uint32_t code = regs.error_code;
        for (int k = 28; k >= 0; k -= 4) {
            uint8_t nibble = (code >> k) & 0xF;
            hex_buf[j++] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
        }
        hex_buf[j] = '\0';
        terminal_writestring(hex_buf);
        terminal_writestring("\n");
    }
    
    terminal_writestring("EIP: 0x");
    char hex_buf[16];
    int j = 0;
    for (int k = 28; k >= 0; k -= 4) {
        uint8_t nibble = (regs.eip >> k) & 0xF;
        hex_buf[j++] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex_buf[j] = '\0';
    terminal_writestring(hex_buf);
    terminal_writestring("\n");

    // Handle specific exceptions with recovery attempts
    switch(int_no) {
        case 0: // Divide by Zero
            terminal_writestring("ERROR: Division by zero attempted. Cannot recover.\n");
            terminal_writestring("========================================\n");
            terminal_writestring("System entering safe halt. Press any key for shell.\n");
            break;
            
        case 13: // General Protection Fault
            terminal_writestring("WARNING: General Protection Fault detected.\n");
            terminal_writestring("Attempting to recover by returning to shell...\n");
            terminal_writestring("========================================\n");
            // In a real kernel, we would restore state and continue
            // For now, return to shell
            return;
            
        case 14: // Page Fault
            terminal_writestring("CRITICAL: Page Fault detected.\n");
            terminal_writestring("CR2 (Faulting Address) information not available in protected mode.\n");
            terminal_writestring("Cannot safely recover. System halting.\n");
            terminal_writestring("========================================\n");
            break;
            
        default:
            if (int_no < 32) {
                terminal_writestring("CPU Exception (unhandled).\n");
                terminal_writestring("Attempting to continue...\n");
                terminal_writestring("========================================\n");
                return; // Try to recover from unknown exceptions
            }
            break;
    }

    // Critical exceptions cause halt
    if (int_no == 0 || int_no == 8 || int_no == 14) {
        while(1) {
            asm volatile("cli; hlt");
        }
    }
}

void irq_handler(registers_t regs) {
    if (regs.int_no == 32) {
        timer_handler(&regs);
    } else if (regs.int_no == 33) {
        keyboard_handler(&regs);
    }

    if (regs.int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}
