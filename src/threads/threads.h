#ifndef THREADS_H
#define THREADS_H

#include <stdint.h>
#include "../display/printf.h"
#include "../memory/heap.h"

#define STACK_SIZE 4096

typedef enum thread_state {
    READY,
    WAITING,
    RUNNING,
    TERMINATED
} thread_state_t;

typedef struct thread {
    uint32_t *stack;
    void (*entry)(void *);
    void *args;
    thread_state_t state;
    struct thread *next;
} thread_t;

extern thread_t *curr_thread;

extern thread_t *ready_queue;

thread_t *create_thread(void (*fn)(void *), void *arg);

extern void switch_thread(uint32_t **old_stack, uint32_t *new_stack);

void schedule();

#endif