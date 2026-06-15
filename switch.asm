section .text
global switch_to_task
global save_context

; void switch_to_task(registers_t* new_task_regs)
; Switches execution to a new task by restoring its CPU state
switch_to_task:
    push ebp
    mov ebp, esp
    
    ; Get pointer to new task registers
    mov eax, [ebp + 8]
    
    ; Restore all general purpose registers
    mov edi, [eax + 0]      ; edi = eax field
    mov esi, [eax + 4]      ; esi = ecx field
    mov ebx, [eax + 8]      ; ebx = edx field
    mov ecx, [eax + 12]     ; ecx = ebx field
    mov edx, [eax + 16]     ; edx = esi field
    mov ebp, [eax + 20]     ; ebp = ebp field
    
    ; Note: we restore ESP and EIP indirectly
    ; esp = [eax + 24]
    ; eip = [eax + 32]
    
    ; Set up new stack
    mov esp, [eax + 24]
    
    ; Restore segment registers
    mov ds, [eax + 52]      ; ds
    mov es, [eax + 56]      ; es  
    mov fs, [eax + 60]      ; fs
    mov gs, [eax + 64]      ; gs
    
    ; Restore EFLAGS
    mov ecx, [eax + 68]
    push ecx
    popf
    
    ; Get the instruction pointer and return to it
    ; We use a trick: push EIP then ret
    mov ecx, [eax + 32]     ; eip
    
    ; Restore remaining registers that we had to save
    mov eax, [eax + 28]     ; restore eax
    
    ; Jump to new task's code
    jmp ecx
    
    mov esp, ebp
    pop ebp
    ret


; void save_context(registers_t* regs)
; Saves current CPU state to registers structure
save_context:
    push ebp
    mov ebp, esp
    
    ; Get pointer to registers structure
    mov eax, [ebp + 8]
    
    ; Save general purpose registers
    mov [eax + 0], edi
    mov [eax + 4], esi
    mov [eax + 8], ebp
    mov [eax + 12], esp
    mov [eax + 16], ebx
    mov [eax + 20], edx
    mov [eax + 24], ecx
    ; eax will be saved by caller
    
    ; Save segment registers
    mov edx, ds
    mov [eax + 52], edx
    mov edx, es
    mov [eax + 56], edx
    mov edx, fs
    mov [eax + 60], edx
    mov edx, gs
    mov [eax + 64], edx
    
    ; Save EFLAGS
    pushf
    pop edx
    mov [eax + 68], edx
    
    mov esp, ebp
    pop ebp
    ret
