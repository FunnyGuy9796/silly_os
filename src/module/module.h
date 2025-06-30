#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>
#include "../misc/string.h"
#include "../display/printf.h"
#include "../kernel/elf.h"
#include "../memory/heap.h"

int load_module(void *module_base);

#endif