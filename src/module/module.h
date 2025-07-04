#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>
#include "../misc/string.h"
#include "../display/printf.h"
#include "../kernel/elf.h"
#include "../memory/heap.h"

typedef struct Module {
    void **section_headers;
    uint16_t num_sections;
    void (*init_func)(struct Module *);
    void (*exit_func)(struct Module *);
    char *file_path;
} module_t;

int load_module(void *module_base, module_t *module);

void unload_module(module_t *module);

#endif