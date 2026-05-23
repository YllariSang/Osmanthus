MODULEALIGN equ  1 << 0
MEMINFO     equ  1 << 1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .text
global _start
extern kernel_main

_start:
    cli                  ; Clear interrupts during stack setup
    mov esp, stack_top   ; Load our system stack pointer
    
    call kernel_main     ; Jump to our C kernel code execution

.kernel_halt_loop:
    hlt                  ; Safe fallback: wait for an interrupt to arrive
    jmp .kernel_halt_loop ; Safely loop backward forever

section .bootstrap_stack nobits
align 16
stack_bottom:
    resb 16384           ; Allocate 16 KiB of safe stack space
stack_top:
