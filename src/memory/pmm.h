#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>
#include "../kernel/multiboot.h"
#include "mmap.h"
#include "../misc/mem.h"

extern uint32_t total_memory;
extern uint32_t frame_size;
extern uint32_t num_frames;
extern uint32_t bitmap_size;
extern uint32_t used_frames;

void pmm_set(uint32_t frame_index);

void pmm_clear(uint32_t frame_index);

uint8_t pmm_test(uint32_t frame_index);

void pmm_init(multiboot_info_t *mb_info, uint32_t k_end);

int32_t pmm_alloc();

void pmm_free(uint32_t frame_index);

uint32_t get_mem_total();

uint32_t get_used_mem();

uint32_t get_free_mem();

#endif