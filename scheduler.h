#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "task.h"

/* Scheduler functions */
void scheduler_init(void);
void scheduler_add_task(task_t* task);
void scheduler_remove_task(task_t* task);
task_t* scheduler_next_task(void);
void scheduler_yield(void);
uint32_t scheduler_get_task_count(void);

#endif
