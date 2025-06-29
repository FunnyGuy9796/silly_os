#ifndef FPU_H
#define FPU_H

static inline void fpu_init() {
    unsigned int cr0;

    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    cr0 &= ~((1 << 2) | (1 << 3));

    __asm__ volatile (
        "mov %0, %%cr0"
        :
        : "r"(cr0)
    );

    __asm__ volatile ("fninit");
}

#endif