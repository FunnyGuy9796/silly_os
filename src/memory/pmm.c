#include "pmm.h"

uint8_t *bitmap;

uint32_t total_memory = 0;
uint32_t frame_size = 4096;
uint32_t num_frames;
uint32_t bitmap_size;

static uint32_t last_alloc = 0;

void pmm_set(uint32_t frame_index) {
    bitmap[frame_index / 8] |= (1 << (frame_index % 8));
}

void pmm_clear(uint32_t frame_index) {
    bitmap[frame_index / 8] &= ~(1 << (frame_index % 8));
}

uint8_t pmm_test(uint32_t frame_index) {
    return (bitmap[frame_index / 8] >> (frame_index % 8)) & 1;
}

void pmm_init(multiboot_info_t *mb_info, uint32_t k_end) {
    uint32_t aligned_k_end = (k_end + frame_size - 1) & ~(frame_size - 1);

    bitmap = (uint8_t *)aligned_k_end;
    total_memory = get_mmap(mb_info);
    num_frames = total_memory / frame_size;
    bitmap_size = (num_frames + 7) / 8;

    memset(bitmap, 0xff, bitmap_size);

    uint32_t mmap_end = mb_info->mmap_addr + mb_info->mmap_length;
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mb_info->mmap_addr;

    while ((uint32_t)mmap < mmap_end) {
        uint64_t base = mmap->base_addr;
        uint64_t length = mmap->length;
        
        if (mmap->type == 1 && base < 0x100000000) {
            uint64_t end = base + length;

            if (end > 0x100000000)
                end = 0x100000000;

            for (uint64_t addr = base; addr < end; addr += frame_size) {
                uint32_t frame = (uint32_t)(addr >> 12);
                
                if (frame < num_frames)
                    pmm_clear(frame);
            }
        }
        
        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    uint32_t kernel_frames = (aligned_k_end + bitmap_size + frame_size - 1) / frame_size;
    for (uint32_t i = 0; i < kernel_frames; i++) {
        pmm_set(i);
    }

    for (uint32_t i = 0; i < 256; i++) {
        pmm_set(i);
    }

    kstatus("debug", "pmm initialized\n\tbitmap stored at address: 0x%x\n\ttotal available memory: %.2fGB\n", aligned_k_end, (double)total_memory / 1073741824.0);
}

int32_t pmm_alloc() {
    for (uint32_t i = 256; i < 1024; i++) {
        if (pmm_test(i) == 0) {
            pmm_set(i);
            
            last_alloc = i + 1;

            return i;
        }
    }

    uint32_t start = (last_alloc < 256) ? 256 : last_alloc;

    for (uint32_t i = start; i < num_frames; i++) {
        if (pmm_test(i) == 0) {
            pmm_set(i);

            last_alloc = i + 1;

            return i;
        }
    }

    for (uint32_t i = 256; i < start; i++) {
        if (pmm_test(i) == 0) {
            pmm_set(i);

            last_alloc = i + 1;

            return i;
        }
    }

    return -1;
}

void pmm_free(uint32_t frame_index) {
    if (frame_index >= num_frames) {
        kstatus("error", "pmm_free(): frame index out of range: %d\n", frame_index);

        return;
    }

    pmm_clear(frame_index);

    if (frame_index < last_alloc)
        last_alloc = frame_index;
}