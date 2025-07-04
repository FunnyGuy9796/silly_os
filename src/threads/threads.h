#ifndef THREADS_H
#define THREADS_H

#include <stdint.h>
#include "../kernel/panic.h"
#include "../display/printf.h"
#include "../timer/timer.h"
#include "../memory/heap.h"

#define STACK_SIZE 4096
#define MAX_THREADS 16
#define TIME_QUANTUM 1

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_TERMINATED,
} thread_state_t;

typedef struct Thread {
    uint32_t esp;
    uint32_t *stack_base;
    int id;
    thread_state_t state;
    uint32_t time_slice;
    uint32_t sleep_time;
    uint32_t last_active;
} thread_t;

extern thread_t threads[MAX_THREADS];
extern thread_t *idle_thread;
extern thread_t *curr_thread;
extern int thread_count;
extern int scheduler_enabled;

thread_t *create_thread(void (*func)(void));
void destroy_thread(thread_t *thread);
void scheduler_init(thread_t *idle);
void switch_to(thread_t *old, thread_t *new);
thread_t *get_next_thread();
void schedule();
void thread_sleep(thread_t *thread, uint32_t ms);
void thread_exit();

#endif