#include "idt.h"

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

extern void idt_load(uint32_t);
extern void isr_dummy();
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr32();

void idt_set_gate(int num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = base & 0xffff;
    idt[num].offset_high = (base >> 16) & 0xffff;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 33; i < IDT_ENTRIES; i++)
        idt_set_gate(i, (uint32_t)isr_dummy, 0x08, 0x8e);
    
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8e);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8e);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8e);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8e);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8e);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8e);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8e);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8e);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8e);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8e);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8e);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8e);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8e);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8e);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8e);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8e);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8e);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8e);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8e);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8e);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8e);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8e);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8e);

    idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8e);
    
    idt_load((uint32_t)&idtp);

    kstatus("debug", "idt initialized\n");
}