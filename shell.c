#include <stdint.h>
#include <stddef.h>

extern void terminal_putchar(char c);
extern void terminal_writestring(const char* data);

static char shell_buffer[256];
static size_t shell_buffer_index = 0;

/* Bare-metal string comparison utility */
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/* Outbound byte port operation helper */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void shell_init(void) {
    shell_buffer_index = 0;
    terminal_writestring("\nOsmanthus Shell Active.\n> ");
}

/* Native x86 CPUID Assembly Engine */
static void print_cpu_info(void) {
    uint32_t eax, ebx, ecx, edx;
    
    /* Call CPUID with EAX = 0 to fetch the Vendor ID string */
    asm volatile("cpuid"
                 : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                 : "a"(0));
                 
    /* The 12-character string is returned across EBX, EDX, and ECX */
    char vendor[13];
    vendor[0] = (char)(ebx & 0xFF);
    vendor[1] = (char)((ebx >> 8) & 0xFF);
    vendor[2] = (char)((ebx >> 16) & 0xFF);
    vendor[3] = (char)((ebx >> 24) & 0xFF);
    
    vendor[4] = (char)(edx & 0xFF);
    vendor[5] = (char)((edx >> 8) & 0xFF);
    vendor[6] = (char)((edx >> 16) & 0xFF);
    vendor[7] = (char)((edx >> 24) & 0xFF);
    
    vendor[8] = (char)(ecx & 0xFF);
    vendor[9] = (char)((ecx >> 8) & 0xFF);
    vendor[10] = (char)((ecx >> 16) & 0xFF);
    vendor[11] = (char)((ecx >> 24) & 0xFF);
    vendor[12] = '\0';

    terminal_writestring("Detected CPU Vendor String: ");
    terminal_writestring(vendor);
    terminal_writestring("\n");
}

/* Processes commands once Enter (\n) is pressed */
static void shell_execute_command(void) {
    shell_buffer[shell_buffer_index] = '\0';

    if (shell_buffer_index == 0) {
        terminal_writestring("\n> ");
        return;
    }

    terminal_writestring("\n");

    // COMMAND INTERPRETER MATRIX
    if (strcmp(shell_buffer, "help") == 0) {
        terminal_writestring("Osmanthus Kernel Supported Operations:\n");
        terminal_writestring("  help       - Display documentation support lists\n");
        terminal_writestring("  version    - Print microkernel release information\n");
        terminal_writestring("  cpuinfo    - Pull hardware processor vendor ID via CPUID\n");
        terminal_writestring("  reboot     - Pulse the 8042 controller to reset the machine\n");
        terminal_writestring("  clear      - Wipe terminal display buffer output\n");
    } 
    else if (strcmp(shell_buffer, "version") == 0) {
        terminal_writestring("Osmanthus OS Core v0.1.0 (32-bit Protected Mode)\n");
    } 
    else if (strcmp(shell_buffer, "cpuinfo") == 0) {
        print_cpu_info();
    }
    else if (strcmp(shell_buffer, "reboot") == 0) {
        terminal_writestring("Pulsing hardware reset lines... Goodbye!\n");
        
        /* Pulse the reset line of the 8042 PS/2 Keyboard Controller.
           Writing 0xFE to port 0x64 pulls the CPU's RESET pin low. */
        outb(0x64, 0xFE);
        
        /* Fallback dead-loop if the chip takes a fraction of a millisecond to assert */
        while(1) { asm volatile("hlt"); }
    }
    else if (strcmp(shell_buffer, "clear") == 0) {
        extern void terminal_initialize(void);
        terminal_initialize();
    } 
    else {
        terminal_writestring("Unknown command: ");
        terminal_writestring(shell_buffer);
        terminal_writestring("\nType 'help' for available actions.");
    }

    shell_buffer_index = 0;
    terminal_writestring("\n> ");
}

void shell_input_char(char c) {
    if (c == '\n') {
        shell_execute_command();
    } 
    else if (c == '\b') {
        if (shell_buffer_index > 0) {
            shell_buffer_index--;
            terminal_putchar('\b');
        }
    } 
    else {
        if (shell_buffer_index < 255) {
            shell_buffer[shell_buffer_index++] = c;
            terminal_putchar(c);
        }
    }
}
