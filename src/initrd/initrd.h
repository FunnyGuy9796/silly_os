#ifndef INITRD_H
#define INITRD_H

#include <stdint.h>
#include <stddef.h>
#include "../misc/string.h"
#include "../kernel/multiboot.h"
#include "../display/printf.h"
#include "../kernel/panic.h"

#define TAR_BLOCK_SIZE 512
#define MAX_ENTRIES 64

typedef struct __attribute__((packed)) {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
} tar_header_t;

extern void *initrd;
extern uint32_t initrd_size;
extern char curr_dir[256];

void initrd_load(multiboot_info_t *mb_info);

char **list_tar(char *this_dir, int *entry_count);

void *read_file(const char *file_name, uint32_t *file_size);

#endif