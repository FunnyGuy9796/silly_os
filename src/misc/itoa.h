#ifndef ITOA_H
#define ITOA_H

static inline void itoa(int value, char *str, int base) {
    if (base < 2 || base > 16) {
        str[0] = '\0';
        return;
    }

    char *digits = "0123456789ABCDEF";
    char buffer[32];
    int i = 0, is_negative = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    while (value && i < (int)(sizeof(buffer) - 1)) {
        buffer[i++] = digits[value % base];
        value /= base;
    }

    if (is_negative)
        buffer[i++] = '-';

    for (int j = 0; j < i; j++)
        str[j] = buffer[i - j - 1];

    str[i] = '\0';
}

#endif