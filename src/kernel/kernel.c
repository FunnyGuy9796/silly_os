#include "../display/display.h"
#include "../display/printf.h"
#include "../init/fpu.h"
#include "../init/gdt.h"
#include "../interrupts/idt.h"

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1BADB002,
    0x00,
    -(0x1BADB002 + 0x00)
};

void kmain() {
    kclear();
    fpu_init();
    gdt_init();
    idt_init();
    enable_interrupts();

    kprintf("[info] interrupts enabled\n");
    
    kprintf("\nsilly OS v%d\n\n", 1);

    kprintf("[warn] nothing to do, halting system...\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}