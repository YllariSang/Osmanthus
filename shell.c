#include <stdint.h>
#include <stddef.h>
#include "graphics.h"

#define SHELL_BUFFER_MAX 256

/* Core Subsystem Callbacks */
extern void print_serial_string(const char* str);
extern void write_serial_char(char c);
extern char read_serial_char(void);
extern void terminal_writestring(const char* data);
extern void terminal_initialize(void);

static char shell_buffer[SHELL_BUFFER_MAX];
static int shell_buffer_index = 0;

/* Asynchronous input buffer latch fed directly by keyboard.c */
volatile char native_key_buffer = '\0';

void shell_input_char(char c) {
    native_key_buffer = c;
}

/* Standard string matching utility */
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void clear_buffer(void) {
    for (int i = 0; i < SHELL_BUFFER_MAX; i++) {
        shell_buffer[i] = '\0';
    }
    shell_buffer_index = 0;
}

void print_prompt(void) {
    print_serial_string("\n\rOsmanthus OS > ");
}

/* Core Command Routing Logic */
void execute_command(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        print_serial_string("\n\rOsmanthus Kernel Supported Operations:\n\r");
        print_serial_string("help       - Display documentation support lists\n\r");
        print_serial_string("version    - Print microkernel release information\n\r");
        print_serial_string("meminfo    - Output live resource tracking utilization charts\n\r");
        print_serial_string("guitest    - Drop text mode and render an interactive canvas (Bypasses Shift)\n\r");
        print_serial_string("clear      - Wipe terminal display buffer output\n\r");
    } 
    else if (strcmp(cmd, "version") == 0) {
        print_serial_string("\n\rOsmanthus Microkernel - v0.4.6 (Direct Input Engine)\n\r");
    } 
    else if (strcmp(cmd, "clear") == 0) {
        terminal_initialize();
    } 
    else if (strcmp(cmd, "meminfo") == 0) {
        print_serial_string("\n\rDynamic Heap Monitoring Engine:\n\r");
        print_serial_string("Subsystem Allocation Pool Arena: 1048576 Bytes (1MB Loaded)\n\r");
        print_serial_string("Status Validation: STEADY\n\r");
    } 
    // Dual Alias Routing: Accept 'gui_test' or 'guitest' to satisfy missing keyboard shift-states
    else if (strcmp(cmd, "gui_test") == 0 || strcmp(cmd, "guitest") == 0) {
        print_serial_string("\n\rDropping Text Mode. Initiating Canvas Graphics... Check window!\n\r");
        
        // 1. Flip VGA register configurations
        graphics_enter_mode13();
        
        // 2. Lock execution focus inside interactive cursor loop
        run_graphics_demo();
        
        // 3. Once user exits via 'Q', reset text mode state cleanly
        terminal_initialize();
        print_serial_string("\n\rReturned safely to text terminal mode environment.\n\r");
    } 
    else if (cmd[0] != '\0') {
        print_serial_string("\n\rUnknown command: ");
        print_serial_string(cmd);
        print_serial_string("\n\rType 'help' for available actions.\n\r");
    }
}

/* Entry point called by kernel_main to manage interactive execution */
void shell_init(void) {
    clear_buffer();
    terminal_initialize();
    
    print_serial_string("\n\r===========================================");
    print_serial_string("\n\r* OSMANTHUS OS NATIVE COMPOSITOR SHELL    *");
    print_serial_string("\n\r===========================================\n\r");
    print_serial_string("Click inside your browser screen window to direct keyboard input tokens.\n\r");
    print_serial_string("Type 'help' to examine available operations.\n\r");
    
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
                    // Visual echo corrective erase backspace pattern over serial
                    write_serial_char(0x08);
                    write_serial_char(' ');
                    write_serial_char(0x08);
                }
            } 
            // Append general typing characters safely up to buffer margins
            else {
                if (shell_buffer_index < SHELL_BUFFER_MAX - 1) {
                    shell_buffer[shell_buffer_index++] = c;
                    write_serial_char(c); // Echo character back to display
                }
            }
        }
        
        // Yield CPU cycles back to hardware gracefully between character polling events
        asm volatile("hlt");
    }
}
