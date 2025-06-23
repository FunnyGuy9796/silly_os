#include "multitasking.h"

scheduler_t scheduler = {0};

thread_t *get_curr_thread() {
    thread_t *curr_thread = scheduler.queue;

    while (curr_thread) {
        if (curr_thread->id == scheduler.curr_id)
            return curr_thread;
        
        curr_thread = curr_thread->next;
    }

    return NULL;
}

void add_thread(thread_t *t) {
    thread_t *curr_thread = scheduler.queue;
    int next_id = 0;

    if (!curr_thread) {
        scheduler.queue = t;
        t->next = NULL;
        t->id = next_id;
        t->status = T_READY;

        return;
    }

    while (curr_thread->next) {
        if (curr_thread->id >= next_id)
            next_id = curr_thread->id + 1;

        curr_thread = curr_thread->next;
    }

    if (curr_thread->id >= next_id)
        next_id = curr_thread->id + 1;

    curr_thread->next = t;
    t->next = NULL;
    t->id = next_id;
    t->status = T_READY;
}

void remove_thread(thread_t *t) {
    thread_t *curr_thread = scheduler.queue;

    while (curr_thread->next) {
        if (curr_thread == t) {
            t->status = T_TERMINATED;

            return;
        }

        curr_thread = curr_thread->next;
    }
}

void scheduler_init() {
    scheduler.curr_id = 0;
    scheduler.queue = NULL;

    kstatus("debug", "scheduler initialized\n");
}