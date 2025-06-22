#include <stdint.h>
#include "multiboot.h"
#include "panic.h"
#include "../display/display.h"
#include "../display/printf.h"
#include "../init/fpu.h"
#include "../init/gdt.h"
#include "../pic/pic.h"
#include "../interrupts/idt.h"
#include "../memory/pmm.h"
#include "../memory/paging.h"
#include "../memory/heap.h"

#define STACK_TOP 0x00201000

extern uint32_t _start;
extern uint32_t _end;

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1BADB002,
    0x00,
    -(0x1BADB002 + 0x00)
};

void kinit(multiboot_info_t *mb_info) {
    kstatus("info", "kernel from 0x%x to 0x%x\n", &_start, &_end);

    fpu_init();
    gdt_init();

    pic_remap();

    idt_init();
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

    uint32_t *a = kmalloc(100, 0);
    uint32_t *b = kmalloc(4096, 1);

    kprintf("a: 0x%p, b: 0x%p\n", a, b);

    kprintf("\nsilly OS v%d\n\n", 1);

    kstatus("warn", "nothing to do, halting system\n");

    while (1)
        __asm__ volatile ("hlt");
}