#include "display/display.h"
#include "../arch/fpu.h"

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1BADB002,
    0x00,
    -(0x1BADB002 + 0x00)
};

void kmain() {
    fpu_init();
    kclear();
    kprint("silly.iso v1.0");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
