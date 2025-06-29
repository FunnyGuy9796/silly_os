#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>
#include "pmm.h"
#include "paging.h"

#define KHEAP_START 0x00400000
#define KHEAP_INIT_SIZE (PAGE_SIZE * 16)

typedef struct kheap_block {
    uint32_t size;
    uint8_t free;
    struct kheap_block *next;
} kheap_block_t;

extern uint32_t heap_end;
extern uint32_t heap_curr;
extern kheap_block_t *free_list;

void kheap_init();

void *kmalloc(uint32_t size, uint8_t align);

void kfree(void *ptr);

void *krealloc(void *ptr, uint32_t new_size);

#endif