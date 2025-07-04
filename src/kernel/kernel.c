#include <stdint.h>
#include "multiboot.h"
#include "elf.h"
#include "panic.h"
#include "../display/display.h"
#include "../display/printf.h"
#include "../arch/fpu.h"
#include "../arch/gdt.h"
#include "../pic/pic.h"
#include "../interrupts/idt.h"
#include "../timer/timer.h"
#include "../memory/pmm.h"
#include "../memory/paging.h"
#include "../memory/heap.h"
#include "../initrd/initrd.h"
#include "../module/module.h"
#include "../threads/threads.h"
#include "../misc/day.h"

extern uint32_t _start;
extern uint32_t _end;

#define MULTIBOOT_MAGIC 0x1badb002
#define MULTIBOOT_FLAGS 0x03
#define MULTIBOOT_CHECKSUM (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS))

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM
};

void kidle() {
    kstatus("debug", "entered idle thread\n\tcurr_thread = %d\n\tthread_count = %d\n", curr_thread->id, thread_count);

    while (1)
        __asm__ volatile("hlt");
}

void ksetup() {
    module_t init_mod;
    init_mod.file_path = "initrd/modules/init/init.ko";
    
    uint32_t size;
    void *data = read_file(init_mod.file_path, &size);

    if (load_module(data, &init_mod) != 0)
        kstatus("error", "kinit(): failed to load module\n");
    else {
        init_mod.init_func(&init_mod);
        init_mod.exit_func(&init_mod);
        
        unload_module(&init_mod);
    }

    thread_exit();
}

void kinit(multiboot_info_t *mb_info) {
    if (!(mb_info->flags & (1 << 3)))
        kpanic("kinit(): no modules loaded by bootloader\n");

    if (mb_info->mods_count <= 0)
        kpanic("kinit(): initrd could not be found\n\tmb_info->mods_cound: %d\n", mb_info->mods_count);
    
    if (!(mb_info->flags & (1 << 5)))
        kpanic("kinit(): no ELF section headers provided by bootloader\n");
    
    multiboot_module_t *modules = (multiboot_module_t *)mb_info->mods_addr;

    kstatus("debug", "kernel from 0x%x to 0x%x\n", &_start, &_end);

    elf_init(mb_info);

    fpu_init();
    gdt_init();

    pic_remap();

    idt_init();
    pit_init(100);
    enable_interrupts();

    pmm_init(mb_info, (uint32_t)&modules[0].mod_end);
    paging_init();

    kheap_init();

    keyboard_init();

    initrd_load(mb_info);

    kclear();
    
    idle_thread = create_thread(&kidle);
    thread_t *init_thread = create_thread(&ksetup);

    scheduler_init(idle_thread);

    kstatus("debug", "waiting for scheduler\n");

    while (1)
        __asm__ volatile("hlt");
}

void kmain(uint32_t magic, uint32_t addr) {
    multiboot_info_t *mb_info = (multiboot_info_t *)addr;
    
    if (magic != 0x2badb002)
        kpanic("kmain(): invalid magic number\n\tmagic: 0x%x\n", magic);

    kinit(mb_info);
}