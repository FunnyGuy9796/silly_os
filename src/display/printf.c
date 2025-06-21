#include "printf.h"

void kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[32];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;
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
                case 'x': {
                    int num = va_arg(args, int);
                    itoa(num, buffer, 16);
                    kprint(buffer);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    kputc(c, row, col);
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
        } else {
            kputc(format[i], row, col);
        }
    }

    va_end(args);
}