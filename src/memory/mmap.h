#ifndef MMAP_H
#define MMAP_H

#include <stdint.h>
#include "../kernel/multiboot.h"
#include "../kernel/panic.h"
#include "../display/printf.h"

uint32_t get_mmap(multiboot_info_t *mb_info);

#endif