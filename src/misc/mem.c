#include "mem.h"

void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;

    while (num--) {
        *p++ = (unsigned char)value;
    }

    return ptr;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;

    for (size_t i = 0; i < n; i++)
        d[i] = s[i];

    return dest;
}