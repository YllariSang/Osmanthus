global gdt_flush

gdt_flush:
    ; Grab the first incoming argument passed via the stack frame (pointer to gdt_ptr)
    mov eax, [esp + 4]
    
    ; Load the Global Descriptor Table register profile
    lgdt [eax]

    ; Reload Data Segment Registers with our flat data selector index (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Perform a Far Jump to reload the Code Segment Register (CS) with index 0x08
    jmp 0x08:.flush_done

.flush_done:
    ; Clean return back to the caller frame without stack misalignment
    ret
