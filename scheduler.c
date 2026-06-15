#include "scheduler.h"
#include "malloc.h"
#include "stdio.h"

static task_t* ready_queue = NULL;
static uint32_t task_count = 0;

void scheduler_init(void) {
    ready_queue = NULL;
    task_count = 0;
}

void scheduler_add_task(task_t* task) {
    if (!task) return;
    
    task->state = TASK_READY;
    
    // If queue is empty, task becomes the head
    if (!ready_queue) {
        ready_queue = task;
        task->next = task;  // Circular queue - points to itself
    } else {
        // Find the tail and insert at the end
        task_t* tail = ready_queue;
        while (tail->next != ready_queue) {
            tail = tail->next;
        }
        task->next = ready_queue;
        tail->next = task;
    }
    
    task_count++;
}

void scheduler_remove_task(task_t* task) {
    if (!task || !ready_queue || task_count == 0) return;
    
    // If only one task, remove it
    if (ready_queue == task && task->next == task) {
        ready_queue = NULL;
        task_count--;
        return;
    }
    
    // Find the task before this one
    task_t* current = ready_queue;
    while (current->next != task && current->next != ready_queue) {
        current = current->next;
    }
    
    if (current->next == task) {
        // Remove task from queue
        current->next = task->next;
        
        // If we removed the head, move head forward
        if (ready_queue == task) {
            ready_queue = task->next;
        }
        
        task->next = NULL;
        task_count--;
    }
}

task_t* scheduler_next_task(void) {
    if (!ready_queue) return NULL;
    
    // Current task has completed its time slice
    // Move to next task in round-robin fashion
    task_t* next = ready_queue->next;
    if (next == ready_queue) {
        // Only one task, no switching needed
        return ready_queue;
    }
    
    // Move the head of the queue forward (round-robin)
    ready_queue = next;
    
    return ready_queue;
}

void scheduler_yield(void) {
    // Force a context switch on next timer tick
    task_t* current = task_get_current();
    if (current) {
        current->ticks_remaining = 0;
    }
}

uint32_t scheduler_get_task_count(void) {
    return task_count;
}
