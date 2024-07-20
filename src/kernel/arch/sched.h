#pragma once

#include <stdint.h>
#include <arch/i686/irq.h>

#define MAX_TASKS 256
#define STACK_SIZE 1024

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_TERMINATED
} task_state_t;

typedef struct {
    uint32_t task_id;
    uint32_t priority;
    uint32_t current_priority;
    task_state_t state;
    uint32_t *stack_pointer; // Pointer to the task's stack
} task_t;

typedef struct {
    task_t tasks[MAX_TASKS];  // Array of tasks
    uint32_t current_task;    // Index of the currently running task
    uint32_t num_tasks;       // Number of tasks currently managed by the scheduler
} scheduler_t;

extern scheduler_t scheduler;
extern uint8_t task_stacks[MAX_TASKS][STACK_SIZE];

void scheduler_init();
uint32_t create_task(void (*task_function)(void), uint32_t priority);
void delete_task(uint32_t task_id);
void yield();
void timer_interrupt_handler(Registers* regs);