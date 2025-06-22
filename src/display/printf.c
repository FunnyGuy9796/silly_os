#include "printf.h"

void kvprintf(const char *format, va_list args) {
    char buffer[64];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;

            int precision = 6;

            if (format[i] == '.') {
                i++;
                precision = 0;

                while (format[i] >= '0' && format[i] <= '9') {
                    precision = precision * 10 + (format[i] - '0');
                    i++;
                }
            }

            switch (format[i]) {
                case 's': {
                    const char *str = va_arg(args, const char*);

                    kprint(str);

                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);

                    itoa(num, buffer, 10);
                    kprint(buffer);

                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);

                    itoa(num, buffer, 10);
                    kprint(buffer);

                    break;
                }
                case 'f': {
                    double num = va_arg(args, double);

                    ftoa(num, buffer, precision);
                    kprint(buffer);

                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);

                    itoa(num, buffer, 16);
                    kprint(buffer);

                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);

                    kputc(c, row, col);

                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void*);
                    uintptr_t addr = (uintptr_t)ptr;

                    itoa(addr, buffer, 16);

                    int len = 0;

                    for (const char *p = buffer; *p; p++) len++;

                    for (int i = len; i < 8; i++)
                        kputc('0', row, col);
                    
                    kprint(buffer);

                    break;
                }
                case '%': {
                    kputc('%', row, col);

                    break;
                }
                default:
                    kputc('%', row, col);
                    kputc(format[i], row, col);

                    break;
            }
        } else
            kputc(format[i], row, col);
    }
}

void kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    kvprintf(format, args);

    va_end(args);
}

void kstatus(const char *category, const char *format, ...) {
    va_list args;
    va_start(args, format);

    if (strcmp(category, "info") == 0)
        kcolor(VGA_BLACK, VGA_CYAN);
    else if (strcmp(category, "warn") == 0)
        kcolor(VGA_BLACK, VGA_YELLOW);
    else if (strcmp(category, "error") == 0)
        kcolor(VGA_BLACK, VGA_RED);
    else
        kcolor(VGA_BLACK, VGA_WHITE);

    kprintf("[");
    kprintf(category);
    kprintf("] ");

    kcolor(VGA_BLACK, VGA_WHITE);

    kvprintf(format, args);

    va_end(args);
}