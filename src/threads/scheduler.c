#include "threads.h"

thread_t threads[MAX_THREADS];
thread_t *idle_thread;
thread_t *curr_thread;
int thread_count = 0;
int scheduler_enabled = 0;
static int next_thread_index = 0;
static int idle_thread_index = -1;

void scheduler_init(thread_t *idle) {
    idle_thread = idle;

    if (!idle_thread)
        kpanic("scheduler_init(): invalid idle_thread\n");

    for (int i = 0; i < thread_count; i++) {
        if (&threads[i] == idle_thread) {
            idle_thread_index = i;

            break;
        }
    }

    curr_thread = idle_thread;
    curr_thread->state = THREAD_RUNNING;
    curr_thread->time_slice = TIME_QUANTUM;
    scheduler_enabled = 1;

    thread_t *dummy = {0};

    switch_to(dummy, curr_thread);

    kstatus("debug", "scheduler initialized\n");
}

void switch_to(thread_t *old, thread_t *new) {
    if (old == new) return;

    __asm__ volatile (
        "cli\n\t"
        "pushf\n\t"
        "pusha\n\t"
        "movl %%esp, (%0)\n\t"
        "movl (%1), %%esp\n\t"
        "popa\n\t"
        "popf\n\t"
        "ret\n\t"
        :
        : "r" (&old->esp), "r" (&new->esp)
        : "memory"
    );
}

thread_t *get_next_thread() {
    if (thread_count < 1) {
        kpanic("get_next_thread(): thread_count < 1\n");

        return idle_thread;
    }

    int start_index = next_thread_index;

    do {
        if (next_thread_index < thread_count && threads[next_thread_index].state == THREAD_READY) {
            next_thread_index = (next_thread_index + 1) % thread_count;

            thread_t *next = &threads[next_thread_index];

            return next;
        }

        next_thread_index = (next_thread_index + 1) % thread_count;
    } while (next_thread_index != start_index);

    return idle_thread;
}

void schedule() {
    if (!scheduler_enabled) return;

    if (curr_thread == NULL)
        kpanic("schedule(): curr_thread = NULL\n");

    if (thread_count == 0)
        kpanic("schedule(): no new threads to schedule\n");

    thread_t *old_thread = curr_thread;
    thread_t *new_thread;

    if (curr_thread != idle_thread && curr_thread->time_slice > 0)
        curr_thread->time_slice--;
    
    if (curr_thread == idle_thread || curr_thread->state == THREAD_RUNNING || curr_thread->time_slice == 0 ||
        curr_thread->state == THREAD_BLOCKED || curr_thread->state != THREAD_TERMINATED) {
        
        if (curr_thread->state == THREAD_RUNNING)
            curr_thread->state = THREAD_READY;
        
        if (curr_thread->state == THREAD_BLOCKED && (curr_thread->last_active + curr_thread->sleep_time) > get_uptime())
            curr_thread->state = THREAD_READY;
        
        new_thread = get_next_thread();

        if (new_thread && new_thread != curr_thread) {
            new_thread->time_slice = TIME_QUANTUM;
            new_thread->state = THREAD_RUNNING;

            curr_thread = new_thread;

            switch_to(old_thread, new_thread);
        } else
            switch_to(old_thread, idle_thread);
    }
}

void thread_sleep(thread_t *thread, uint32_t ms) {
    thread->state = THREAD_BLOCKED;
    thread->last_active = get_uptime();
    thread->sleep_time = ms;
}