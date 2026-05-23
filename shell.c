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

void shell_init(void) {
    shell_buffer_index = 0;
    terminal_writestring("\nOsmanthus Shell Active.\n> ");
}

/* Processes commands once Enter (\n) is pressed */
static void shell_execute_command(void) {
    shell_buffer[shell_buffer_index] = '\0'; // Null-terminate string buffer

    if (shell_buffer_index == 0) {
        terminal_writestring("\n> ");
        return;
    }

    terminal_writestring("\n");

    // COMMAND VALIDATION ENGINE
    if (strcmp(shell_buffer, "help") == 0) {
        terminal_writestring("Osmanthus Kernel Supported Operations:\n");
        terminal_writestring("  help       - Display documentation support lists\n");
        terminal_writestring("  version    - Print microkernel release information\n");
        terminal_writestring("  clear      - Wipe terminal display buffer output\n");
    } 
    else if (strcmp(shell_buffer, "version") == 0) {
        terminal_writestring("Osmanthus OS Core v0.1.0 (32-bit Protected Mode)\n");
    } 
    else if (strcmp(shell_buffer, "clear") == 0) {
        // External reference hack to reinitialize screen layout rows
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

/* Receives ASCII input directly from the keyboard driver interrupt layer */
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
