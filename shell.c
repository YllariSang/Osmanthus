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

/* Forward declarations of our system memory allocator routines */
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);
extern void kmalloc_get_stats(size_t* total_free, size_t* total_used, size_t* overhead);

/* Custom printer to output base-10 integers without a standard printf library */
static void print_number(size_t n) {
    if (n == 0) {
        terminal_putchar('0');
        return;
    }
    char buf[12];
    int i = 10;
    buf[11] = '\0';
    while (n > 0 && i >= 0) {
        buf[i--] = (n % 10) + '0';
        n /= 10;
    }
    terminal_writestring(&buf[i + 1]);
}

void shell_init(void) {
    shell_buffer_index = 0;
    terminal_writestring("\nOsmanthus Shell Active.\n> ");
}

/* Advanced CPUID Brand Extractor */
static void print_cpu_info(void) {
    uint32_t eax, ebx, ecx, edx;
    
    /* Step 1: Base Vendor Identification */
    asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    char vendor[13];
    *(uint32_t*)&vendor[0] = ebx;
    *(uint32_t*)&vendor[4] = edx;
    *(uint32_t*)&vendor[8] = ecx;
    vendor[12] = '\0';

    terminal_writestring("Processor Vendor: ");
    terminal_writestring(vendor);
    terminal_writestring("\n");

    /* Step 2: Model Name String Extraction */
    uint32_t brand[12];
    asm volatile("cpuid" : "=a"(brand[0]), "=b"(brand[1]), "=c"(brand[2]), "=d"(brand[3]) : "a"(0x80000002));
    asm volatile("cpuid" : "=a"(brand[4]), "=b"(brand[5]), "=c"(brand[6]), "=d"(brand[7]) : "a"(0x80000003));
    asm volatile("cpuid" : "=a"(brand[8]), "=b"(brand[9]), "=c"(brand[10]), "=d"(brand[11]) : "a"(0x80000004));

    terminal_writestring("Model Specification: ");
    char* brand_str = (char*)brand;
    for (int i = 0; i < 48; i++) {
        if (brand_str[i] >= 32 && brand_str[i] <= 126) {
            terminal_putchar(brand_str[i]);
        }
    }
    terminal_writestring("\n");
}

/* SysAdmin style RAM metrics report function */
static void print_mem_info(void) {
    size_t free_ram, used_ram, kernel_overhead;
    kmalloc_get_stats(&free_ram, &used_ram, &kernel_overhead);

    terminal_writestring("Osmanthus Kernel Memory Status:\n");
    terminal_writestring("  Allocated RAM: "); print_number(used_ram);        terminal_writestring(" bytes\n");
    terminal_writestring("  Available Heap: "); print_number(free_ram);        terminal_writestring(" bytes\n");
    terminal_writestring("  Block Overhead: "); print_number(kernel_overhead); terminal_writestring(" bytes\n");
    terminal_writestring("  Total Heap Managed: "); print_number(used_ram + free_ram + kernel_overhead); terminal_writestring(" bytes\n");
}

/* Processes commands once Enter (\n) is pressed */
static void shell_execute_command(void) {
    shell_buffer[shell_buffer_index] = '\0';

    if (shell_buffer_index == 0) {
        terminal_writestring("\n> ");
        return;
    }

    terminal_writestring("\n");

    // EXTENDED COMMAND INTERPRETER MATRIX
    if (strcmp(shell_buffer, "help") == 0) {
        terminal_writestring("Osmanthus Kernel Supported Operations:\n");
        terminal_writestring("  help        - Display documentation support lists\n");
        terminal_writestring("  version     - Print microkernel release information\n");
        terminal_writestring("  cpuinfo     - Pull comprehensive hardware architecture brand strings\n");
        terminal_writestring("  meminfo     - Output live resource tracking utilization charts\n");
        terminal_writestring("  malloc_test - Dynamically allocate, write, and free Heap space\n");
        terminal_writestring("  gui_test    - Drop text mode and render a custom 256-color graphics canvas\n");
        terminal_writestring("  reboot      - Pulse the 8042 controller to reset the machine\n");
        terminal_writestring("  clear       - Wipe terminal display buffer output\n");
    } 
    else if (strcmp(shell_buffer, "version") == 0) {
        terminal_writestring("Osmanthus OS Core v0.1.0 (32-bit Protected Mode)\n");
    } 
    else if (strcmp(shell_buffer, "cpuinfo") == 0) {
        print_cpu_info();
    }
    else if (strcmp(shell_buffer, "meminfo") == 0) {
        print_mem_info();
    }
    else if (strcmp(shell_buffer, "malloc_test") == 0) {
        print_mem_info();
        terminal_writestring("\nRequesting 2 dynamic pointers from the heap...\n");
        
        char* ptr1 = (char*)kmalloc(1024); /* Claim 1 KB */
        char* ptr2 = (char*)kmalloc(2048); /* Claim 2 KB */
        
        print_mem_info();
        
        terminal_writestring("\nReleasing heap blocks...\n");
        kfree(ptr1);
        kfree(ptr2);
        
        print_mem_info();
    }
    else if (strcmp(shell_buffer, "gui_test") == 0) {
        terminal_writestring("Dropping Text Mode. Initiating Canvas Graphics... Check window!\n");
        
        extern void graphics_enter_mode13(void);
        extern void graphics_clear_screen(uint8_t color);
        extern void draw_rectangle(int x, int y, int width, int height, uint8_t color);
        
        /* Initialize hardware registers with safe clock waits */
        graphics_enter_mode13();
        
        /* Paint canvas background to solid bright blue (VGA Color 9) */
        graphics_clear_screen(9);
        
        /* Draw our geometric canvas accents */
        draw_rectangle(40,  40,  60, 60, 40);  // Red block
        draw_rectangle(120, 40,  80, 40, 15);  // Pure white block
        draw_rectangle(220, 60,  50, 90, 14);  // Yellow block
        draw_rectangle(80,  110, 120, 50, 2);  // Green block
        
        /* Hold execution here so the screen state persists */
        while(1) { asm volatile("hlt"); }
    }
    else if (strcmp(shell_buffer, "reboot") == 0) {
        terminal_writestring("Pulsing hardware reset lines... Goodbye!\n");
        outb(0x64, 0xFE);
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
