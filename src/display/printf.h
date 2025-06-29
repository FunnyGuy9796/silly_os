#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>
#include "../misc/itoa.h"
#include "../misc/ftoa.h"
#include "../misc/string.h"
#include "../initrd/initrd.h"
#include "display.h"

void kvprintf(const char *format, va_list args);

void kprintf(const char *format, ...);

void kstatus(const char *category, const char *format, ...);

void kprompt(const char *user);

int ksnprintf(char *buf, size_t size, const char *format, ...);

#endif