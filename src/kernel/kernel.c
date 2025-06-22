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

extern uint32_t _start;
extern uint32_t _end;

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1BADB002,
    0x00,
    -(0x1BADB002 + 0x00)
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
}

void kmain(uint32_t magic, uint32_t addr) {
    if (magic != 0x2badb002) {
        kclear();
        kstatus("error", "invalid magic number: 0x%x\n", magic);

        kpanic("invalid magic number\n\tmagic: 0x%x\n", magic);
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

    kstatus("info", "current time: %02u:%02u:%02u %s\n\tday: %02u\n\tmonth: %02u\n\tyear: %02u\n",
        n_hours, curr_time.minutes, curr_time.seconds, apm, curr_time.day, curr_time.month, curr_time.year);
}