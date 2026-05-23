; --- MULTIBOOT HEADER SPECIFICATION ---
MBOOT_PAGE_ALIGN    equ 1 << 0
MBOOT_MEM_INFO      equ 1 << 1
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM

section .text
global _start
extern kernel_main

_start:
    ; Establish a rock-solid, isolated temporary stack pointer at the 4MB boundary mark.
    ; This keeps the stack thousands of bytes clear of our variables or code sections!
    mov esp, 0x00400000
    
    ; Reset EFLAGS to clear any leftover nested task loops or interrupt states
    push 0
    popf

    ; Call our kernel main without forcing any misaligned argument pushes onto our stack pointer
    call kernel_main

.hang:
    hlt
    jmp .hang
