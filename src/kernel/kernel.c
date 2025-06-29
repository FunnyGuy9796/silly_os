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
#include "../misc/day.h"

extern uint32_t _start;
extern uint32_t _end;

char input[128];

#define MULTIBOOT_MAGIC 0x1badb002
#define MULTIBOOT_FLAGS 0x03
#define MULTIBOOT_CHECKSUM (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS))

__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM
};

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
}

void kmain(uint32_t magic, uint32_t addr) {
    multiboot_info_t *mb_info = (multiboot_info_t *)addr;
    
    if (magic != 0x2badb002)
        kpanic("kmain(): invalid magic number\n\tmagic: 0x%x\n", magic);

    kinit(mb_info);

    char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    rtc_time_t curr_time = get_sys_time();
    char *apm = "AM";
    uint8_t n_hours = curr_time.hours;
    int day_num = get_day(curr_time.day, curr_time.month, curr_time.year);

    if (curr_time.hours > 12) {
        n_hours -= 12;
        apm = "PM";
    } else if (curr_time.hours == 0)
        n_hours = 12;
    
    kprintf("\n");

    kstatus("info", "%s %s %d %02u:%02u:%02u %s %02u\n",
        day[day_num], month[curr_time.month - 1], curr_time.day, n_hours, curr_time.minutes, curr_time.seconds, apm, curr_time.year);
}