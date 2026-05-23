global gdt_flush

gdt_flush:
    mov eax, [esp+4]  ; Get the pointer to the GDT structure passed as an argument
    lgdt [eax]        ; Execute the Load GDT CPU instruction

    ; Reload data segment pointers with our new data offset (0x10 is entry index 2)
    mov ax, 0x10      
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Execute a far jump to reload code segment cache register (0x08 is entry index 1)
    jmp 0x08:.flush
.flush:
    ret
