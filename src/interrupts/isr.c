#include "isr.h"

void isr_handler(registers_t *regs) {
    if (regs->int_no <= 30) {
        kcolor(VGA_RED, VGA_WHITE);
        kclear();

        kprintf("exception %d occurred\n", regs->int_no);
        kprintf("EIP: 0x%x\n", regs->eip);
        kprintf("error code: 0x%x\n", regs->err_code);

        kprintf("\nregisters:\n");
        kprintf("EAX: 0x%x  EBX: 0x%x  ECX: 0x%x  EDX: 0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
        kprintf("ESI: 0x%x  EDI: 0x%x  EBP: 0x%x  ESP: 0x%x\n", regs->esi, regs->edi, regs->ebp, regs->esp);
        kprintf("CS: 0x%x  DS: 0x%x  EFLAGS: 0x%x\n", regs->cs, regs->ds, regs->eflags);

        if (regs->int_no == 14) {
            uint32_t faulting_addr;

            __asm__ volatile ("mov %%cr2, %0" : "=r"(faulting_addr));

            kprintf("CR2: 0x%x\n", faulting_addr);
        }

        while (1)
            __asm__ volatile ("hlt");
    } else {
        switch (regs->int_no) {
            case 32: {
                timer_callback();

                break;
            }
        }

        if (regs->int_no >= 32 && regs->int_no <= 47)
            pic_send_eoi(regs->int_no - 32);
    }
}