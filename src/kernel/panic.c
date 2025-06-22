#include "panic.h"

void kpanic(const char *format, ...) {
    va_list args;
    va_start(args, format);

    kcolor(VGA_BLUE, VGA_WHITE);
    kclear();

    kprintf("kernel panic\n\tthe kernel encountered an unrecoverable error, please reboot...\n\n");

    kvprintf(format, args);

    while (1)
        __asm__ volatile ("hlt");

    va_end(args);
}