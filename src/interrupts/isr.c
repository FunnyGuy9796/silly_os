#include "isr.h"

void isr_handler(registers_t *regs) {
    kcolor(VGA_RED, VGA_WHITE);
    kclear();

    kprintf("Exception %d occurred\n", regs->int_no);
    kprintf("EIP: 0x%x\n", regs->eip);
    kprintf("Error code: 0x%x\n", regs->err_code);

    kprintf("Registers:\n");
    kprintf("EAX: 0x%x  EBX: 0x%x  ECX: 0x%x  EDX: 0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI: 0x%x  EDI: 0x%x  EBP: 0x%x  ESP: 0x%x\n", regs->esi, regs->edi, regs->ebp, regs->esp);
    kprintf("CS: 0x%x  DS: 0x%x  EFLAGS: 0x%x\n", regs->cs, regs->ds, regs->eflags);

    while (1) {
        __asm__ volatile ("hlt");
    }
}