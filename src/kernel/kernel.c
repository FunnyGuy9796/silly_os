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
#include "../multitasking/multitasking.h"

extern uint32_t _start;
extern uint32_t _end;

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1badb002,
    0x00,
    -(0x1badb002 + 0x00)
};

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

    scheduler_init();
}

void kthread() {
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

    while (1) {
        __asm__ volatile ("hlt");
    }
}

void kmain(uint32_t magic, uint32_t addr) {
    if (magic != 0x2badb002) {
        kclear();
        kstatus("error", "invalid magic number: 0x%x\n", magic);

        kpanic("kmain(): invalid magic number\n\tmagic: 0x%x\n", magic);
    }

    multiboot_info_t *mb_info = (multiboot_info_t *)addr;

    kinit(mb_info);

    thread_t k_thread;

    new_thread(&k_thread, kthread);

    kstatus("debug", "switching to kernel thread\n");

    cpu_context_t dummy_context;

    kprintf("k_thread.context:\n\tebx: 0x%x  esi: 0x%x  edi: 0x%x\n\tebp: 0x%x  esp: 0x%x  eip: 0x%x\n",
        k_thread.context.ebx, k_thread.context.esi, k_thread.context.edi, k_thread.context.ebp, k_thread.context.esp, k_thread.context.eip);

    uint32_t *sp = (uint32_t *)k_thread.context.esp;
    *sp = 0xDEADBEEF;

    kprintf("switching to thread:\n");
    kprintf("  esp: 0x%08x (stack base: 0x%08x)\n", k_thread.context.esp, (uint32_t)k_thread.stack);
        
    switch_context(&dummy_context, &k_thread.context);

    kpanic("kmain(): kernel thread exited\n");
}