#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>
#include "../misc/itoa.h"
#include "display.h"

void kprintf(const char *format, ...);

#endif