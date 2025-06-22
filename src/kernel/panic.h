#ifndef PANIC_H
#define PANIC_H

#include <stdarg.h>
#include "../display/printf.h"

void kpanic(const char *format, ...);

#endif