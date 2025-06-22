#include "itoa.h"

void itoa(unsigned int value, char *str, int base) {
    if (base < 2 || base > 16) {
        str[0] = '\0';
        return;
    }

    char *digits = "0123456789abcdef";
    char buffer[32];
    int i = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (value && i < (int)(sizeof(buffer) - 1)) {
        buffer[i++] = digits[value % base];
        value /= base;
    }

    for (int j = 0; j < i; j++)
        str[j] = buffer[i - j - 1];

    str[i] = '\0';
}