#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>
#include "../display/display.h"
#include "../memory/heap.h"
#include "../misc/mem.h"

#define STACK_SIZE 4096

typedef enum thread_status {
    T_READY,
    T_RUNNING,
    T_BLOCKED,
    T_TERMINATED
} thread_status_t;

typedef struct cpu_context {
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t eip;
} cpu_context_t;

struct thread;

typedef struct thread {
    cpu_context_t context;
    void *stack;
    void (*func)();
    thread_status_t status;
    int id;
    struct thread *next;
} thread_t;

typedef struct scheduler {
    int curr_id;
    thread_t *queue;
} scheduler_t;

extern scheduler_t scheduler;

extern void switch_context(cpu_context_t* c_old, cpu_context_t* c_new);

thread_t *get_curr_thread();

void exit_thread();

void new_thread(thread_t *t, void (*func)());

void add_thread(thread_t *t);

void remove_thread(thread_t *t);

void scheduler_init();

#endif