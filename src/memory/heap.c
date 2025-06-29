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

    kstatus("debug", "kernel heap initialized\n\tstarting address: 0x%x\n", KHEAP_START);
}

void ensure_kheap_mapping(void *addr, uint32_t size) {
    uint32_t start = (uint32_t)addr & ~(PAGE_SIZE - 1);
    uint32_t end = ((uint32_t)addr + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    for (uint32_t va = start; va < end; va += PAGE_SIZE) {
        if (!get_page_entry(page_directory, va)) {
            int frame = pmm_alloc();

            if (frame == -1)
                kpanic("ensure_kheap_mapping(): out of memory\n");
            
            map_page(page_directory, va, frame * PAGE_SIZE, PAGE_PRESENT | PAGE_RW);
        }
    }

    __asm__ volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax", "memory");
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

    kstatus("debug", "kernel heap expanded: 0x%x\n", heap_end);
}

void *kmalloc(uint32_t size, uint8_t align) {
    if (size == 0) return NULL;

    kheap_block_t* curr = free_list;
    kheap_block_t* prev = NULL;

    while (curr) {
        if (curr->free && curr->size >= size) {
            ensure_kheap_mapping(curr, sizeof(kheap_block_t) + curr->size + sizeof(void*));

            uint32_t data_addr = (uint32_t)curr + sizeof(kheap_block_t) + sizeof(void*);
            uint32_t aligned_addr = align ? ((data_addr + 0xFFF) & ~0xFFF) : data_addr;
            uint32_t padding = aligned_addr - data_addr;

            if (curr->size < size + padding + sizeof(void*))
                goto next;

            if (padding) {
                kheap_block_t *padding_block = (kheap_block_t *)((uint8_t *)curr + sizeof(kheap_block_t));
                padding_block->size = padding - sizeof(kheap_block_t) - sizeof(void*);
                padding_block->free = 1;
                padding_block->next = curr->next;

                curr->size = (uint8_t *)padding_block - (uint8_t *)curr - sizeof(kheap_block_t);
                curr->next = padding_block;
                curr = padding_block;
            }

            if (curr->size > size + sizeof(kheap_block_t) + sizeof(void*)) {
                kheap_block_t *new_block = (kheap_block_t *)(aligned_addr + size + sizeof(void*));
                new_block->size = curr->size - (size + padding + sizeof(void*) + sizeof(kheap_block_t));
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size + padding + sizeof(void*);
                curr->next = new_block;
            }

            curr->free = 0;

            ((kheap_block_t **)aligned_addr)[-1] = curr;

            return (void *)aligned_addr;
        }
    next:
        prev = curr;
        curr = curr->next;
    }

    uint32_t expand_size = size + sizeof(kheap_block_t) + sizeof(void*) + (align ? PAGE_SIZE : 0);
    expand_size = (expand_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    uint32_t old_heap_end = heap_end;
    kheap_exp(heap_end + expand_size);

    kheap_block_t *new_block = (kheap_block_t *)old_heap_end;
    ensure_kheap_mapping(new_block, expand_size);
    new_block->size = expand_size - sizeof(kheap_block_t);
    new_block->free = 1;
    new_block->next = NULL;

    if (prev)
        prev->next = new_block;
    else
        free_list = new_block;

    return kmalloc(size, align);
}

void kfree(void *ptr) {
    if (!ptr) return;

    kheap_block_t *block = ((kheap_block_t **)ptr)[-1];

    block->free = 1;

    kheap_block_t *curr = free_list;

    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(kheap_block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void *krealloc(void *ptr, uint32_t new_size) {
    if (!ptr)
        return kmalloc(new_size, 0);

    if (new_size == 0) {
        kfree(ptr);

        return NULL;
    }

    kheap_block_t *block = ((kheap_block_t **)ptr)[-1];
    uint32_t old_size = block->size - sizeof(void*);

    if (new_size <= old_size) {
        uint32_t excess_size = old_size - new_size;

        if (excess_size > sizeof(kheap_block_t) + sizeof(void*)) {
            kheap_block_t *new_block = (kheap_block_t *)((uint8_t *)ptr + new_size);

            new_block->size = excess_size - sizeof(kheap_block_t);
            new_block->free = 1;
            new_block->next = block->next;

            block->size = new_size + sizeof(void*);
            block->next = new_block;

            kheap_block_t *curr = free_list;

            while (curr && curr->next) {
                if (curr->free && curr->next->free) {
                    curr->size += sizeof(kheap_block_t) + curr->next->size;
                    curr->next = curr->next->next;
                } else
                    curr = curr->next;
            }
        }

        return ptr;
    } else {
        kheap_block_t *next_block = block->next;

        if (next_block && next_block->free &&
            (block->size + sizeof(kheap_block_t) + next_block->size) >= (new_size + sizeof(void*))) {

            block->size += sizeof(kheap_block_t) + next_block->size;
            block->next = next_block->next;

            uint32_t excess_size = block->size - (new_size + sizeof(void*));

            if (excess_size > sizeof(kheap_block_t) + sizeof(void*)) {
                kheap_block_t *new_block = (kheap_block_t *)((uint8_t *)ptr + new_size);
                new_block->size = excess_size - sizeof(kheap_block_t);
                new_block->free = 1;
                new_block->next = block->next;

                block->size = new_size + sizeof(void*);
                block->next = new_block;
            }

            return ptr;
        } else {
            void *new_ptr = kmalloc(new_size, 0);

            if (!new_ptr) return NULL;

            memcpy(new_ptr, ptr, old_size);
            kfree(ptr);

            return new_ptr;
        }
    }
}