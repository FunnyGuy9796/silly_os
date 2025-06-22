#include "heap.h"

uint32_t heap_end;
uint32_t heap_curr;
kheap_block_t *free_list = NULL;

void kheap_init() {
    for (uint32_t i = 0; i < KHEAP_INIT_SIZE; i += PAGE_SIZE) {
        int frame = pmm_alloc();

        if (frame == -1)
            kpanic("kheap_init(): out of physical memory\n");

        map_page(page_directory, KHEAP_START + i, frame * PAGE_SIZE, PAGE_PRESENT | PAGE_RW);
    }

    asm volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax", "memory");

    heap_end = KHEAP_START + KHEAP_INIT_SIZE;

    free_list = (kheap_block_t *)KHEAP_START;
    free_list->size = KHEAP_INIT_SIZE - sizeof(kheap_block_t);
    free_list->free = 1;
    free_list->next = NULL;

    kstatus("info", "kernel heap initialized\n\tstarting address: 0x%x\n", KHEAP_START);
}

void kheap_exp(uint32_t new_size) {
    if (new_size <= heap_end)
        return;

    while (heap_end < new_size) {
        int frame = pmm_alloc();

        if (frame == -1)
            kpanic("kheap_exp(): out of physical memory\n");
        
        uint32_t phys = frame * PAGE_SIZE;
        
        map_page(page_directory, heap_end, phys, PAGE_PRESENT | PAGE_RW);

        heap_end += PAGE_SIZE;
    }

    kstatus("info", "kernel heap expanded: 0x%x\n", heap_end);
}

void *kmalloc(uint32_t size, uint8_t align) {
    if (size == 0) return NULL;

    if (align)
        size = (size + 0xfff) & ~0xfff;
    
    kheap_block_t* curr = free_list;
    kheap_block_t* prev = NULL;

    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size >= size + sizeof(kheap_block_t) + 4) {
                kheap_block_t *new_block = (kheap_block_t *)((uint8_t *)curr + sizeof(kheap_block_t) + size);
                new_block->size = curr->size - size - sizeof(kheap_block_t);
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }

            curr->free = 0;

            return (void *)((uint8_t *)curr + sizeof(kheap_block_t));
        }

        prev = curr;
        curr = curr->next;
    }

    uint32_t expand_size = size + sizeof(kheap_block_t);

    expand_size = (expand_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    uint32_t old_heap_end = heap_end;

    kheap_exp(heap_end + expand_size);

    kheap_block_t *new_block = (kheap_block_t *)old_heap_end;
    new_block->size = expand_size - sizeof(kheap_block_t);
    new_block->free = 1;
    new_block->next = NULL;

    if (prev)
        prev->next = new_block;
    else
        free_list = new_block;
    
    return (void *)((uint8_t *)new_block + sizeof(kheap_block_t));
}

void kfree(void *ptr) {
    if (!ptr)
        return;
    
    kheap_block_t *block = (kheap_block_t *)((uint8_t *)ptr - sizeof(kheap_block_t));
    block->free = 1;

    kheap_block_t *curr = free_list;

    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(kheap_block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else
            curr = curr->next;
    }
}