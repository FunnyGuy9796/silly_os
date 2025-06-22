#include "paging.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void paging_init() {
    memset(page_directory, 0, sizeof(page_directory));

    for (int i = 0; i < 1024; i++)
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;

    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

    static uint32_t second_page_table[1024] __attribute__((aligned(4096)));

    memset(second_page_table, 0, sizeof(second_page_table));

    for (int i = 0; i < 1024; i++)
        second_page_table[i] = ((i + 1024) * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;

    page_directory[1] = ((uint32_t)second_page_table) | PAGE_PRESENT | PAGE_RW;

    asm volatile("mov %0, %%cr3" :: "r"(page_directory));

    uint32_t cr0;

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

    kstatus("info", "paging initialized\n\tfirst 4MB of memory identity mapped\n");
}

uint32_t *create_page_table() {
    int frame = pmm_alloc();

    if (frame == -1) {
        kpanic("create_page_table(): no free memory available\n");

        return NULL;
    }

    uint32_t phys = frame * PAGE_SIZE;

    uint32_t *page_table = (uint32_t *)phys_to_virt(phys);

    memset(page_table, 0, PAGE_SIZE);

    return page_table;
}

void map_page_table(uint32_t *page_directory, int dir_index, uint32_t *page_table, uint32_t flags) {
    uint32_t phys = virt_to_phys((uint32_t)page_table);
    
    page_directory[dir_index] = phys | flags;
}

void map_page(uint32_t *page_directory, uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    uint32_t dir_index = (virtual_addr >> 22) & 0x3FF;
    uint32_t table_index = (virtual_addr >> 12) & 0x3FF;
    uint32_t *page_table;

    if (!(page_directory[dir_index] & PAGE_PRESENT)) {
        page_table = create_page_table();

        if (!page_table) {
            kpanic("map_page(): failed to allocate page table\n");

            return;
        }

        map_page_table(page_directory, dir_index, page_table, PAGE_PRESENT | PAGE_RW);
    } else {
        uint32_t phys = page_directory[dir_index] & 0xFFFFF000;

        page_table = (uint32_t *)phys_to_virt(phys);
    }

    page_table[table_index] = (physical_addr & 0xFFFFF000) | flags;

    asm volatile("invlpg (%0)" :: "r"(virtual_addr) : "memory");
}

void unmap_page(uint32_t *page_directory, uint32_t virt_addr) {
    uint32_t dir_index = (virt_addr >> 22) & 0x3FF;
    uint32_t table_index = (virt_addr >> 12) & 0x3FF;
    
    if (!(page_directory[dir_index] & PAGE_PRESENT))
        return;
    
    uint32_t phys = page_directory[dir_index] & 0xFFFFF000;
    uint32_t *page_table = (uint32_t *)phys_to_virt(phys);

    page_table[table_index] = 0;

    asm volatile("invlpg (%0)" :: "r"(virt_addr) : "memory");
}

uint32_t vaddr_to_paddr(uint32_t *page_directory, uint32_t virt_addr) {
    uint32_t dir_index = (virt_addr >> 22) & 0x3FF;
    uint32_t table_index = (virt_addr >> 12) & 0x3FF;
    
    if (!(page_directory[dir_index] & PAGE_PRESENT))
        return 0;
    
    uint32_t phys = page_directory[dir_index] & 0xFFFFF000;
    uint32_t *page_table = (uint32_t *)phys_to_virt(phys);
    
    if (!(page_table[table_index] & PAGE_PRESENT))
        return 0;
    
    return (page_table[table_index] & 0xFFFFF000) | (virt_addr & 0xFFF);
}