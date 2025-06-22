#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

void *memset(void *ptr, int value, size_t num);

void *memcpy(void *dest, const void *src, size_t n);

#endif