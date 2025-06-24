#include "threads.h"

thread_t *curr_thread;
thread_t *ready_queue;

void schedule() {
    if (!ready_queue)
        return;

    thread_t *old_thread = curr_thread;

    do {
        curr_thread = curr_thread->next;
    } while (curr_thread->state != READY);

    switch_thread(&old_thread->stack, curr_thread->stack);

    kpanic("schedule(): unable to switch to new thread\n");
}

void thread_exit() {
    curr_thread->state = TERMINATED;

    while (1)
        __asm__ volatile("hlt");
}

void thread_trampoline(void (*fn)(void *), void *arg) {
    fn(arg);
    thread_exit();
}

thread_t *create_thread(void (*fn)(void *), void *arg) {
    thread_t *t = kmalloc(sizeof(thread_t), 0);

    if (t == NULL)
        kpanic("create_thread(): failed to allocate thread\n");

    t->stack = kmalloc(STACK_SIZE, 0);

    if (t->stack == NULL)
        kpanic("create_thread(): failed to allocate stack\n");

    uint32_t *sp = (uint32_t *)((uint8_t *)t->stack + STACK_SIZE);

    *--sp = (uint32_t)arg;
    *--sp = (uint32_t)fn;
    *--sp = (uint32_t)thread_trampoline;
    *--sp = 0;

    t->stack = sp;
    t->state = READY;

    return t;
}