#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stddef.h>
#include "idt.h"

/* Task states */
typedef enum {
    TASK_READY = 0,
    TASK_RUNNING = 1,
    TASK_BLOCKED = 2,
    TASK_ZOMBIE = 3
} task_state_t;

/* Task Control Block - represents a single task/process */
typedef struct task {
    uint32_t id;                    /* Unique task ID */
    task_state_t state;             /* Current task state */
    registers_t* registers;         /* Saved CPU state */
    uint32_t* stack;                /* Task's stack pointer */
    uint32_t stack_size;            /* Stack size in bytes */
    uint32_t kernel_stack;          /* Kernel stack for interrupts */
    uint32_t parent_id;             /* Parent task ID (for processes) */
    uint32_t ticks_remaining;       /* Time slice remaining */
    struct task* next;              /* Next task in queue (linked list) */
    char name[32];                  /* Task name for debugging */
} task_t;

/* Task management functions */
void task_init(void);
task_t* task_create(void (*entry_point)(void), const char* name, uint32_t stack_size);
void task_destroy(task_t* task);
task_t* task_get_current(void);
void task_set_current(task_t* task);
uint32_t task_get_id_counter(void);

#endif
