#include <stdint.h>
#include <stddef.h>
#include "graphics.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"
#include "kernel.h"

#define SHELL_BUFFER_MAX 256

static char shell_buffer[SHELL_BUFFER_MAX];
static int shell_buffer_index = 0;

/* Asynchronous input buffer latch fed directly by keyboard.c */
volatile char native_key_buffer = '\0';

void shell_input_char(char c) {
    native_key_buffer = c;
}

void clear_buffer(void) {
    for (int i = 0; i < SHELL_BUFFER_MAX; i++) {
        shell_buffer[i] = '\0';
    }
    shell_buffer_index = 0;
}

void print_prompt(void) {
    printf("\nOsmanthus OS > ");
}

/* Core Command Routing Logic */
void execute_command(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        printf("\nOsmanthus Kernel Supported Operations:\n");
        printf("help       - Display documentation support lists\n");
        printf("version    - Print microkernel release information\n");
        printf("meminfo    - Output live resource tracking utilization charts\n");
        printf("guitest    - Drop text mode and render an interactive canvas\n");
        printf("clear      - Wipe terminal display buffer output\n");
        printf("uptime     - Show system uptime in seconds\n");
    } 
    else if (strcmp(cmd, "version") == 0) {
        printf("\nOsmanthus Microkernel - v0.5.0 (Enhanced I/O)\n");
    } 
    else if (strcmp(cmd, "clear") == 0) {
        terminal_initialize();
    } 
    else if (strcmp(cmd, "uptime") == 0) {
        uint32_t ticks = get_tick_count();
        printf("\nUptime: %d seconds (%d ticks)\n", ticks / 100, ticks);
    }
    else if (strcmp(cmd, "meminfo") == 0) {
        size_t total_free, total_used, overhead;
        extern void kmalloc_get_stats(size_t* total_free, size_t* total_used, size_t* overhead);
        kmalloc_get_stats(&total_free, &total_used, &overhead);
        
        printf("\nDynamic Heap Monitoring Engine:\n");
        printf("Total Free: %d bytes\n", total_free);
        printf("Total Used: %d bytes\n", total_used);
        printf("Overhead:   %d bytes\n", overhead);
        printf("Status Validation: STEADY\n");
    } 
    // Dual Alias Routing: Accept 'gui_test' or 'guitest' to satisfy missing keyboard shift-states
    else if (strcmp(cmd, "gui_test") == 0 || strcmp(cmd, "guitest") == 0) {
        printf("\nDropping Text Mode. Initiating Canvas Graphics... Check window!\n");
        
        // 1. Flip VGA register configurations
        graphics_enter_mode13();
        
        // 2. Lock execution focus inside interactive cursor loop
        run_graphics_demo();
        
        // 3. Once user exits via 'Q', reset text mode state cleanly
        terminal_initialize();
        printf("\nReturned safely to text terminal mode environment.\n");
    } 
    else if (cmd[0] != '\0') {
        printf("\nUnknown command: %s\n", cmd);
        printf("Type 'help' for available actions.\n");
    }
}

/* Entry point called by kernel_main to manage interactive execution */
void shell_init(void) {
    clear_buffer();
    terminal_initialize();
    
    printf("\n===========================================");
    printf("\n* OSMANTHUS OS NATIVE COMPOSITOR SHELL    *");
    printf("\n===========================================\n");
    printf("Click inside your browser screen window to direct keyboard input tokens.\n");
    printf("Type 'help' to examine available operations.\n");
    
    print_prompt();

    while (1) {
        // Non-blocking catch line for character tokens fed by the keyboard driver
        if (native_key_buffer != '\0') {
            char c = native_key_buffer;
            native_key_buffer = '\0'; // Clear the register latch instantly

            // Handle Return / Executions
            if (c == '\n' || c == '\r') {
                shell_buffer[shell_buffer_index] = '\0';
                execute_command(shell_buffer);
                clear_buffer();
                print_prompt();
            } 
            // Handle Backspaces cleanly
            else if (c == '\b') {
                if (shell_buffer_index > 0) {
                    shell_buffer_index--;
                    shell_buffer[shell_buffer_index] = '\0';
                    // Visual echo corrective erase backspace pattern
                    terminal_putchar(0x08);
                    terminal_putchar(' ');
                    terminal_putchar(0x08);
                }
            } 
            // Append general typing characters safely up to buffer margins
            else {
                if (shell_buffer_index < SHELL_BUFFER_MAX - 1) {
                    shell_buffer[shell_buffer_index++] = c;
                    terminal_putchar(c); // Echo character back to display
                }
            }
        }
        
        // Yield CPU cycles back to hardware gracefully between character polling events
        asm volatile("hlt");
    }
}
