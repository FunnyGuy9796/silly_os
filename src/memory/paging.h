#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "../misc/mem.h"
#include "../display/printf.h"
#include "../memory/pmm.h"
#include "../kernel/panic.h"

#define PAGE_SIZE 4096
#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

#define KERNEL_PHYS_OFFSET 0x00100000

#define phys_to_virt(addr) ((void *)((uint32_t)(addr) + KERNEL_PHYS_OFFSET))
#define virt_to_phys(addr) ((uint32_t)(addr) - KERNEL_PHYS_OFFSET)

extern uint32_t page_directory[1024] __attribute__((aligned(4096)));
extern uint32_t first_page_table[1024] __attribute__((aligned(4096)));

uint32_t *create_page_table();

void map_page_table(uint32_t *page_directory, int dir_index, uint32_t *page_table, uint32_t flags);

void map_page(uint32_t *page_directory, uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

void unmap_page(uint32_t *page_directory, uint32_t virt_addr);

uint32_t vaddr_to_paddr(uint32_t *page_directory, uint32_t virt_addr);

void paging_init();

uint32_t *get_page_entry(uint32_t *page_directory, uint32_t virt_addr);

#endif