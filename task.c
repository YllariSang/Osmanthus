#include "task.h"
#include "malloc.h"
#include "string.h"

static task_t* current_task = NULL;
static uint32_t next_task_id = 1;

void task_init(void) {
    current_task = NULL;
}

task_t* task_create(void (*entry_point)(void), const char* name, uint32_t stack_size) {
    // Allocate task structure
    task_t* task = (task_t*)kmalloc(sizeof(task_t));
    if (!task) return NULL;
    
    // Allocate kernel stack
    uint32_t kernel_stack = (uint32_t)kmalloc(4096);
    if (!kernel_stack) {
        kfree(task);
        return NULL;
    }
    
    // Allocate user stack
    uint32_t* user_stack = (uint32_t*)kmalloc(stack_size);
    if (!user_stack) {
        kfree((void*)kernel_stack);
        kfree(task);
        return NULL;
    }
    
    // Allocate registers structure
    registers_t* regs = (registers_t*)kmalloc(sizeof(registers_t));
    if (!regs) {
        kfree(user_stack);
        kfree((void*)kernel_stack);
        kfree(task);
        return NULL;
    }
    
    // Initialize task structure
    task->id = next_task_id++;
    task->state = TASK_READY;
    task->registers = regs;
    task->stack = user_stack;
    task->stack_size = stack_size;
    task->kernel_stack = kernel_stack;
    task->parent_id = 0;
    task->ticks_remaining = 5;  // Default time slice: 5 timer ticks
    task->next = NULL;
    strncpy(task->name, name, 31);
    task->name[31] = '\0';
    
    // Initialize registers
    memset(regs, 0, sizeof(registers_t));
    
    // Set up initial stack pointer to top of user stack
    regs->esp = (uint32_t)user_stack + stack_size - 4;
    
    // Set up initial instruction pointer
    regs->eip = (uint32_t)entry_point;
    
    // Set up segment registers
    regs->cs = 0x08;   // Kernel code segment
    regs->ds = 0x10;   // Kernel data segment
    regs->ss = 0x10;   // Stack segment
    
    // Set up EFLAGS (enable interrupts)
    regs->eflags = 0x202;  // IF flag (interrupt enable) and reserved bit
    
    // Set up EBP
    regs->ebp = regs->esp;
    
    return task;
}

void task_destroy(task_t* task) {
    if (!task) return;
    
    if (task->stack) {
        kfree(task->stack);
    }
    if (task->kernel_stack) {
        kfree((void*)task->kernel_stack);
    }
    if (task->registers) {
        kfree(task->registers);
    }
    kfree(task);
}

task_t* task_get_current(void) {
    return current_task;
}

void task_set_current(task_t* task) {
    current_task = task;
}

uint32_t task_get_id_counter(void) {
    return next_task_id;
}
