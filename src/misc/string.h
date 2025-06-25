#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int strcmp(const char *s1, const char *s2);

char *strchr(const char *s, int c);

char *strtok(char *str, const char *delim);

#endif