#include "printf.h"

void kvprintf(const char *format, va_list args) {
    char buffer[64];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;

            int zero_pad = 0;
            int width = 0;

            while (format[i] == '0') {
                zero_pad = 1;
                i++;
            }

            while (format[i] >= '0' && format[i] <= '9') {
                width = width * 10 + (format[i] - '0');
                i++;
            }

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

                    int len = 0;
                    
                    for (const char *p = buffer; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        kputc(zero_pad ? '0' : ' ');

                    kprint(buffer);

                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);

                    itoa(num, buffer, 10);

                    int len = 0;
                    
                    for (const char *p = buffer; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        kputc(zero_pad ? '0' : ' ');

                    kprint(buffer);

                    break;
                }
                case 'f': {
                    double num = va_arg(args, double);

                    ftoa(num, buffer, precision);

                    int len = 0;
                    
                    for (const char *p = buffer; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        kputc(zero_pad ? '0' : ' ');

                    kprint(buffer);

                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);

                    itoa(num, buffer, 16);

                    int len = 0;
                    
                    for (const char *p = buffer; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        kputc(zero_pad ? '0' : ' ');

                    kprint(buffer);

                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);

                    kputc(c);

                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void*);
                    uintptr_t addr = (uintptr_t)ptr;

                    itoa(addr, buffer, 16);

                    int len = 0;

                    for (const char *p = buffer; *p; p++) len++;

                    for (int i = len; i < 8; i++)
                        kputc('0');
                    
                    kprint(buffer);

                    break;
                }
                case '%': {
                    kputc('%');

                    break;
                }
                default:
                    kputc('%');
                    kputc(format[i]);

                    break;
            }
        } else
            kputc(format[i]);
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
    else if (strcmp(category, "debug") == 0)
        kcolor(VGA_BLACK, VGA_GREEN);
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

void kprompt(const char *user) {
    kcolor(VGA_BLACK, VGA_GREEN);
    kprintf("%s", user);
    kcolor(VGA_BLACK, VGA_WHITE);
    kprintf(":");
    kcolor(VGA_BLACK, VGA_GREEN);
    kprintf("%s", curr_dir);
    kcolor(VGA_BLACK, VGA_WHITE);
    kprintf("> ");
}

int kvsnprintf(char *buf, size_t size, const char *format, va_list args) {
    char *buf_ptr = buf;
    size_t remaining = size;
    int total_count = 0;

    char temp[64];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;

            int zero_pad = 0;
            int width = 0;

            while (format[i] == '0') {
                zero_pad = 1;
                i++;
            }

            while (format[i] >= '0' && format[i] <= '9') {
                width = width * 10 + (format[i] - '0');
                i++;
            }

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
                    int len = 0;

                    for (const char *p = str; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        buf_kputc(&buf_ptr, &remaining, zero_pad ? '0' : ' ', &total_count);
                    
                    for (; *str; str++)
                        buf_kputc(&buf_ptr, &remaining, *str, &total_count);

                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);

                    itoa(num, temp, 10);

                    int len = 0;
                    
                    for (const char *p = temp; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        buf_kputc(&buf_ptr, &remaining, zero_pad ? '0' : ' ', &total_count);
                    
                    for (const char *p = temp; *p; p++)
                        buf_kputc(&buf_ptr, &remaining, *p, &total_count);

                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);

                    itoa(num, temp, 10);

                    int len = 0;
                    
                    for (const char *p = temp; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        buf_kputc(&buf_ptr, &remaining, zero_pad ? '0' : ' ', &total_count);
                    
                    for (const char *p = temp; *p; p++)
                        buf_kputc(&buf_ptr, &remaining, *p, &total_count);

                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);

                    itoa(num, temp, 16);

                    int len = 0;
                    
                    for (const char *p = temp; *p; p++) len++;

                    for (int pad = len; pad < width; pad++)
                        buf_kputc(&buf_ptr, &remaining, zero_pad ? '0' : ' ', &total_count);
                    
                    for (const char *p = temp; *p; p++)
                        buf_kputc(&buf_ptr, &remaining, *p, &total_count);

                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);

                    buf_kputc(&buf_ptr, &remaining, c, &total_count);

                    break;
                }
                case '%': {
                    buf_kputc(&buf_ptr, &remaining, '%', &total_count);

                    break;
                }
                default:
                    buf_kputc(&buf_ptr, &remaining, '%', &total_count);
                    buf_kputc(&buf_ptr, &remaining, format[i], &total_count);

                    break;
            }
        } else
            buf_kputc(&buf_ptr, &remaining, format[i], &total_count);
    }

    if (remaining > 0)
        *buf_ptr = '\0';
    else if (size > 0)
        buf[size - 1] = '\0';

    return total_count;
}

int ksnprintf(char *buf, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int ret = kvsnprintf(buf, size, format, args);

    va_end(args);

    return ret;
}