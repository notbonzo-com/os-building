#include <arch/sched.h>
#include <arch/driver/pmm.h>
#include <arch/i686/irq.h>
#include <arch/i686/pic.h>
#include <arch/i686/io.h>
#include <string.h>
#include <debug.h>
#include <memory.h>
#include <stdbool.h>

uint8_t task_stacks[MAX_TASKS][STACK_SIZE];
scheduler_t scheduler;

/**
 * @brief Mantinance task that runs when no other tasks are ready
 */
void kernel_reaper()
{
    while (1) {
        debugf("Reaping kernel...\n");
        __asm__ volatile ("pause\n");
        yield();
    }
}

inline __attribute__((always_inline)) void context_switch(uint32_t next_task_id, uint32_t eipret) {
    if (scheduler.current_task == next_task_id) {
        return;
    }

    __asm__ volatile (
		"push %2\n"
        "pusha\n"
        "pushf\n"
        "mov %%esp, %0\n"
        "mov %1, %%esp\n"
        : "=m" (scheduler.tasks[scheduler.current_task].stack_pointer)
        : "m" (scheduler.tasks[next_task_id].stack_pointer), "r" (eipret)
    );

    scheduler.current_task = next_task_id;

    __asm__ volatile (
        "popf\n"
        "popa\n"
        "sti\n"
        "ret"
    );
}

void initialize_stack(uint32_t *stack_pointer, void (*task_function)(void)) {
    stack_pointer--; // EIP
    *stack_pointer = (uint32_t)task_function;

    stack_pointer--; // EFLAGS
    *stack_pointer = 0x202; // Default flags value (interrupt enabled)

    // Pusha pushes in order: EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    stack_pointer -= 8; // Space for PUSHA
    stack_pointer[0] = 0; // EDI
    stack_pointer[1] = 0; // ESI
    stack_pointer[2] = 0; // EBP
    stack_pointer[3] = (uint32_t)stack_pointer; // ESP
    stack_pointer[4] = 0; // EBX
    stack_pointer[5] = 0; // EDX
    stack_pointer[6] = 0; // ECX
    stack_pointer[7] = 0; // EAX

    scheduler.tasks[scheduler.num_tasks - 1].stack_pointer = stack_pointer;
}

void scheduler_init() {
    memset(&scheduler, 0, sizeof(scheduler));
    // i686_IRQ_RegisterHandler(0, timer_interrupt_handler); //TODO Fix the interrupt based scheduler

    create_task(kernel_reaper, 0);

    create_task((void (*)(void))__builtin_return_address(0), 1);
    scheduler.current_task = 1;

    yield();
}

uint32_t create_task(void (*task_function)(void), uint32_t priority) {
    if (scheduler.num_tasks >= MAX_TASKS) {
        return -1;
    }

    uint32_t task_id = scheduler.num_tasks++;
    task_t *task = &scheduler.tasks[task_id];

    task->task_id = task_id;
    task->priority = priority;
    task->current_priority = priority;
    task->state = TASK_READY;
    task->stack_pointer = (uint32_t *)&task_stacks[task_id][STACK_SIZE - 1];

    initialize_stack(task->stack_pointer, task_function);

    return task_id;
}

void delete_task(uint32_t task_id) {
    if (task_id >= scheduler.num_tasks) {
        return;
    }
    if (task_id == 0) {
        return; // You can't kill the reaper, sir
    }

    scheduler.tasks[task_id].state = TASK_TERMINATED;
    memset(&task_stacks[task_id][0], 0, STACK_SIZE);
    scheduler.num_tasks--;
    if (scheduler.current_task == task_id) {
        yield();
    }
}

void yield() {
    i686_DisableInterrupts();

    uint32_t highest_priority = 0;
    uint32_t next_task = scheduler.current_task;
    bool found_ready_task = false;

    for (uint32_t i = 1; i < scheduler.num_tasks; i++) {
        if (scheduler.tasks[i].state == TASK_READY) {
            if (!found_ready_task || scheduler.tasks[i].current_priority > highest_priority) {
                highest_priority = scheduler.tasks[i].current_priority;
                next_task = i;
                found_ready_task = true;
            }
            if (i != scheduler.current_task) {
                scheduler.tasks[i].current_priority++;
            }
        }
    }

    if (!found_ready_task) {
        next_task = 0;
    }

    scheduler.tasks[next_task].current_priority = scheduler.tasks[next_task].priority;

    context_switch(next_task, (uintptr_t)__builtin_return_address(0));
}

void schedule(Registers* regs) {
    uint32_t highest_priority = 0;
    uint32_t next_task = scheduler.current_task;
    bool found_ready_task = false;

    for (uint32_t i = 1; i < scheduler.num_tasks; i++) {
        if (scheduler.tasks[i].state == TASK_READY) {
            if (!found_ready_task || scheduler.tasks[i].current_priority > highest_priority) {
                highest_priority = scheduler.tasks[i].current_priority;
                next_task = i;
                found_ready_task = true;
            }
            if (i != scheduler.current_task) {
                scheduler.tasks[i].current_priority++;
            }
        }
    }

    if (!found_ready_task) {
        next_task = 0;
    }

    scheduler.tasks[next_task].current_priority = scheduler.tasks[next_task].priority;

    if (scheduler.current_task != next_task) {
        uint32_t *current_stack_pointer = scheduler.tasks[scheduler.current_task].stack_pointer;
        current_stack_pointer--;
        *current_stack_pointer = regs->eip;
        current_stack_pointer--;
        *current_stack_pointer = regs->eax;
        current_stack_pointer--;
        *current_stack_pointer = regs->ecx;
        current_stack_pointer--;
        *current_stack_pointer = regs->edx;
        current_stack_pointer--;
        *current_stack_pointer = regs->ebx;
        current_stack_pointer--;
        *current_stack_pointer = regs->esp; // Old ESP (garbage)
        current_stack_pointer--;
        *current_stack_pointer = regs->ebp;
        current_stack_pointer--;
        *current_stack_pointer = regs->esi;
        current_stack_pointer--;
        *current_stack_pointer = regs->edi;
        current_stack_pointer--;
        *current_stack_pointer = regs->eflags;
        current_stack_pointer--;
        *current_stack_pointer = regs->esp;

        scheduler.tasks[scheduler.current_task].stack_pointer = current_stack_pointer - 11;
        scheduler.current_task = next_task;
        uint32_t *next_stack_pointer = scheduler.tasks[scheduler.current_task].stack_pointer;

        regs->esp = *next_stack_pointer++;
        regs->eflags = *next_stack_pointer++;
        regs->edi = *next_stack_pointer++;
        regs->esi = *next_stack_pointer++;
        regs->ebp = *next_stack_pointer++;
        next_stack_pointer++; // Skip over the garbage ESP value, don't restore it
        regs->ebx = *next_stack_pointer++;
        regs->edx = *next_stack_pointer++;
        regs->ecx = *next_stack_pointer++;
        regs->eax = *next_stack_pointer++;
        regs->eip = *next_stack_pointer++;

        scheduler.tasks[scheduler.current_task].stack_pointer = next_stack_pointer;
    }
}

void timer_interrupt_handler(Registers* regs) {
    schedule(regs);
    i686_PIC_SendEndOfInterrupt(0);
}