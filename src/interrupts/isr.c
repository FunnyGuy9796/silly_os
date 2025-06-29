#include "isr.h"

char *exceptions[] = {
    "division error", "debug", "non-maskable interrupt", "breakpoint", "overflow", "bound range exceeded", "invalid opcode", "device not available",
    "double fault", "obsolete", "invalid tss", "segment not present", "stack-segment fault", "general protection fault", "page fault", "reserved",
    "x87 floating-point exception", "alignment check", "machine check", "simd floating-point exception", "virtualization exception", "control protection exception",
    "reserved", "hypervisor injection exception", "vmm communication exception", "security exception", "reserved"
};

void isr_handler(registers_t *regs) {
    if (regs->int_no <= 30) {
        kcolor(VGA_BLUE, VGA_WHITE);
        kclear();

        kprintf("%s occurred (%d)\n", exceptions[regs->int_no], regs->int_no);
        kprintf("error code: 0x%x\n", regs->err_code);

        kprintf("\nregisters:\n");
        kprintf("EAX: 0x%08x  EBX: 0x%08x  ECX: 0x%08x  EDX: 0x%08x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
        kprintf("ESI: 0x%08x  EDI: 0x%08x  EBP: 0x%08x  ESP: 0x%08x\n", regs->esi, regs->edi, regs->ebp, regs->esp);
        kprintf("CS: 0x%08x  DS: 0x%08x  EFLAGS: 0x%08x\n", regs->cs, regs->ds, regs->eflags);

        if (regs->int_no == 14) {
            uint32_t faulting_addr;

            __asm__ volatile ("mov %%cr2, %0" : "=r"(faulting_addr));

            kprintf("CR2: 0x%08x\n", faulting_addr);
        }

        uint32_t offset;
        const char *sym_name = get_symbol(regs->eip, &offset);

        kprintf("\nexception at 0x%08x -> %s()\n", regs->eip, sym_name);

        while (1)
            __asm__ volatile ("hlt");
    } else {
        switch (regs->int_no) {
            case 32: {
                timer_callback();

                break;
            }

            case 33: {
                keyboard_callback();

                break;
            }
        }

        if (regs->int_no >= 32 && regs->int_no <= 47)
            pic_send_eoi(regs->int_no - 32);
    }
}