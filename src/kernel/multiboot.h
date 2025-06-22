#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4]; // union of a.out and ELF
    uint32_t mmap_length;
    uint32_t mmap_addr;
    // ... (other fields if needed)
} __attribute__((packed)) multiboot_info_t;

typedef struct multiboot_memory_map {
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type; // 1 = usable, 2 = reserved, etc.
} __attribute__((packed)) multiboot_memory_map_t;

#endif