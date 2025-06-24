#include <stdint.h>
#include "multiboot.h"
#include "panic.h"
#include "../display/display.h"
#include "../display/printf.h"
#include "../init/fpu.h"
#include "../init/gdt.h"
#include "../pic/pic.h"
#include "../interrupts/idt.h"
#include "../timer/timer.h"
#include "../memory/pmm.h"
#include "../memory/paging.h"
#include "../memory/heap.h"
#include "../threads/threads.h"

extern uint32_t _start;
extern uint32_t _end;

uint32_t *kernel_sp;

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1badb002,
    0x00,
    -(0x1badb002 + 0x00)
};

void save_ksp() {
    __asm__ volatile("movl %%esp, %0" : "=r"(kernel_sp));
}

void kinit(multiboot_info_t *mb_info) {
    kstatus("debug", "kernel from 0x%x to 0x%x\n", &_start, &_end);

    fpu_init();
    gdt_init();

    pic_remap();

    idt_init();
    pit_init(100);
    enable_interrupts();

    pmm_init(mb_info, (uint32_t)&_end);
    paging_init();

    kheap_init();
}

void test1(void *arg) {
    kprintf("thread 1 starting with arg = %p\n", arg);
    kprintf("thread 1 exiting\n");
}

void test2(void *arg) {
    kprintf("thread 2 starting with arg = %p\n", arg);
    kprintf("thread 2 exiting\n");
}

void kmain(uint32_t magic, uint32_t addr) {
    save_ksp();

    if (magic != 0x2badb002) {
        kclear();
        kstatus("error", "invalid magic number: 0x%x\n", magic);

        kpanic("kmain(): invalid magic number\n\tmagic: 0x%x\n", magic);
    }

    multiboot_info_t *mb_info = (multiboot_info_t *)addr;

    kinit(mb_info);

    kprintf("\nsilly OS v%.1f\n\n", 1.0);

    rtc_time_t curr_time = get_sys_time();
    char *apm = "AM";
    uint8_t n_hours = curr_time.hours;

    if (curr_time.hours > 12) {
        n_hours -= 12;
        apm = "PM";
    }

    kstatus("info", "current time: %02u-%02u-%02u %02u:%02u:%02u %s\n",
        curr_time.year, curr_time.month, curr_time.day, n_hours, curr_time.minutes, curr_time.seconds, apm);

    sleep(5000);

    curr_time = get_sys_time();
    apm = "AM";
    n_hours = curr_time.hours;

    if (curr_time.hours > 12) {
        n_hours -= 12;
        apm = "PM";
    }

    kstatus("info", "current time: %02u-%02u-%02u %02u:%02u:%02u %s\n",
        curr_time.year, curr_time.month, curr_time.day, n_hours, curr_time.minutes, curr_time.seconds, apm);
    
    thread_t *t1 = create_thread(test1, (void *)0x1234);
    thread_t *t2 = create_thread(test1, (void *)0x4321);

    ready_queue = t1;
    t1->next = t2;
    t2->next = t1;

    curr_thread = t1;

    switch_thread(&kernel_sp, t1->stack);

    while (1) {
        __asm__ volatile ("hlt");
    }
}