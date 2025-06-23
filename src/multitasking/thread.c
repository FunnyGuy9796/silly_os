#include "multitasking.h"

__attribute__((noreturn)) void exit_thread() {
    thread_t *curr_thread = get_curr_thread();

    if (!curr_thread)
        kpanic("exit_thread(): the current thread is null\n");

    curr_thread->status = T_TERMINATED;

    thread_t *next = scheduler.queue;

    while (next && next->status != T_READY)
        next = next->next;
    
    if (!next) {
        kpanic("exit_thread(): no threads left to run\n");
    }

    switch_context(&curr_thread->context, &next->context);
    scheduler.curr_id = next->id;
}

void thread_trampoline() {
    thread_t *curr_thread = get_curr_thread();

    if (curr_thread && curr_thread->func) {
        curr_thread->func();
    }

    exit_thread();

    kpanic("thread_trampoline(): thread did not exit properly\n");
}

void new_thread(thread_t *t, void (*func)()) {
    t->stack = kmalloc(STACK_SIZE, 1);

    uint32_t stack_page_va = ((uint32_t)t->stack) & ~(PAGE_SIZE - 1);
    kprintf("new_thread(): stack mapped? PDE: 0x%x\n", page_directory[stack_page_va >> 22]);

    kprintf("new_thread(): kmalloc returned: 0x%08x\n", (uint32_t)t->stack);

    if (!t->stack) return;

    memset(t->stack, 0, STACK_SIZE);

    uint32_t stack_top = (uint32_t)t->stack + STACK_SIZE;

    kprintf("new_thread(): stack_top: 0x%08x\n", stack_top);
    kprintf("page directory entry for stack: 0x%x\n", page_directory[stack_top >> 22]);

    stack_top -= sizeof(uint32_t);
    *(uint32_t *)stack_top = 0;

    memset(&t->context, 0, sizeof(cpu_context_t));
    t->context.esp = stack_top;
    t->context.ebp = 0;
    t->context.eip = (uint32_t)thread_trampoline;

    t->func = func;
    t->id = scheduler.curr_id + 1;

    scheduler.curr_id = t->id;

    kprintf("new_thread():\n\tstack: %p, stack_top: 0x%x, esp: 0x%x\n", t->stack, stack_top, t->context.esp);
}