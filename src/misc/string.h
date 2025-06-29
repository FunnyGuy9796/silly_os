#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include "mem.h"
#include "../memory/heap.h"

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, size_t n);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, size_t n);

char *strchr(const char *s, int c);

char *strrchr(const char *s, int c);

char *strtok(char *str, const char *delim);

char *strcat(char *dest, const char *src);

char *strncat(char *dest, const char *src, unsigned int n);

size_t strlen(const char *str);

char *strdup(const char *s);

char *strndup(const char *src, size_t n);

#endif