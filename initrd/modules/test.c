#include "api.h"

int data = 23;
int bss[10];

void module_init() {
    kprintf("hello from module_init()\n");
}