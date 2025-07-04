#include "threads.h"

void thread_exit() {
    if (curr_thread == idle_thread)
        kpanic("thread_exit(): idle_thread exited\n");

    curr_thread->state = THREAD_TERMINATED;
    schedule();

    kpanic("thread_exit(): the kernel entered an invalid state\n");
}

thread_t *create_thread(void (*func)(void)) {
    if (thread_count >= MAX_THREADS)
        kpanic("create_thread(): max number of threads reached\n");

    thread_t *new_thread = &threads[thread_count];
    uint32_t *stack_base = kmalloc(STACK_SIZE, 1);

    if (!stack_base)
        kpanic("create_thread(): failed to allocate memory for thread stack\n");

    uint32_t *stack_top = stack_base + (STACK_SIZE / sizeof(uint32_t));

    *--stack_top = (uint32_t)func;
    *--stack_top = 0x0202;
    *--stack_top = 0;
    *--stack_top = 0;
    *--stack_top = 0;
    *--stack_top = 0;
    *--stack_top = 0;
    *--stack_top = 0;
    *--stack_top = 0;
    *--stack_top = 0;

    new_thread->esp = (uint32_t)stack_top;
    new_thread->stack_base = stack_base;
    new_thread->id = thread_count;
    new_thread->state = THREAD_READY;
    new_thread->time_slice = TIME_QUANTUM;

    thread_count++;

    return new_thread;
}

void destroy_thread(thread_t *thread) {
    thread->state = THREAD_TERMINATED;
    kfree(thread->stack_base);

    for (int i = 0; i < thread_count; i++) {
        if (threads[i].id == thread->id) {
            if (i < thread_count - 1)
                threads[i] = threads[thread_count - 1];
            
            thread_count--;
            
            if (thread == curr_thread)
                schedule();
            
            return;
        }
    }
    
    kstatus("error", "destroy_thread(): thread not found\n");
}