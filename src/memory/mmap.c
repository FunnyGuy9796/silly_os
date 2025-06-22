#include "mmap.h"

uint32_t get_mmap(multiboot_info_t *mb_info) {
    if (mb_info->flags & (1 << 6)) {
        kstatus("debug", "found memory map\n");

        uint32_t mmap_end = mb_info->mmap_addr + mb_info->mmap_length;
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mb_info->mmap_addr;
        uint32_t total_memory = 0;

        while ((uint32_t)mmap < mmap_end) {
            total_memory += (uint32_t)mmap->length;
            
            mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
        }

        return total_memory;
    } else
        kstatus("warn", "no memory map found\n");

        kpanic("get_mmap(): no memory map found\n");

    return 0;
}