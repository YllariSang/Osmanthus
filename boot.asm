; --- MULTIBOOT FLAGS SPECIFICATION CONFIGURATION ---
MBOOT_PAGE_ALIGN    equ 1 << 0
MBOOT_MEM_INFO      equ 1 << 1
MBOOT_GRAPHICS_MODE equ 1 << 2  ; Request GRUB to initialize a video canvas layout!
MBOOT_FLAGS         equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_GRAPHICS_MODE
MBOOT_MAGIC         equ 0x1BADB002
MBOOT_CHECKSUM      equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM
    
    ; GRUB Address fallbacks (Required pads for Multiboot compliance)
    dd 0, 0, 0, 0, 0
    
    ; Target Video Mode Request Parameters
    dd 0    ; 0 = Linear graphics frame buffer mode type
    dd 320  ; Target Resolution Width Max Boundary
    dd 200  ; Target Resolution Height Max Boundary
    dd 8    ; Target Color Bit-Depth Format (8-bit = 256 Colors index)

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB reserved for system stack space
stack_top:

section .text
global _start
_start:
    ; Set up our secure stack pointer frame layout
    mov esp, stack_top

    ; Pass control directly into your C microkernel entry point
    extern kernel_main
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
