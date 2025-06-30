#include "api.h"

int data = 23;
int bss[10];

void module_init() {
    kprintf("initrd/modules/test.ko -- module loaded successfully\n");

    void *addr = kmalloc(1024, 0);

    if (addr) {
        kprintf("initrd/modules/test.ko -- allocated 1024 bytes at 0x%x\n", (uint32_t)addr);
    
        kfree(addr);
    }
}