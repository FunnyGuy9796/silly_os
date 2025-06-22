#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include "../display/display.h"
#include "../display/printf.h"

typedef struct regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip, cs, eflags;
} registers_t;

void isr_handler(registers_t *regs);

#endif