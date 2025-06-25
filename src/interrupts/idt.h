#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "../misc/byte.h"
#include "../display/printf.h"
#include "../devices/keyboard.h"

#define IDT_ENTRIES 256

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void enable_interrupts();

void idt_set_gate(int num, uint32_t base, uint16_t selector, uint8_t flags);

void idt_init();

#endif