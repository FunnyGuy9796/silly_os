#include "initrd.h"

void *initrd = NULL;
uint32_t initrd_size = 0;
char curr_dir[256];

uint32_t otod(const char *str) {
    uint32_t result = 0;

    while (*str >= '0' && *str <= '7')
        result = (result << 3) + (*str++ - '0');
    
    return result;
}

void initrd_load(multiboot_info_t *mb_info) {
    multiboot_module_t *modules = (multiboot_module_t *)mb_info->mods_addr;
    
    initrd = (void *)modules[0].mod_start;
    initrd_size = modules[0].mod_end - modules[0].mod_start;

    if (initrd == NULL || initrd_size <= 0)
        kpanic("load_initrd(): initrd NULL or has size of 0\n");
    
    uint8_t *ptr = (uint8_t *)initrd;
    tar_header_t *header = (tar_header_t *)ptr;

    strcpy(curr_dir, "initrd/");
    
    kstatus("debug", "initrd loaded\n\tstarting address: 0x%x\n\tsize: %d bytes\n", initrd, initrd_size);
}

char **list_tar(char *this_dir, int *entry_count) {
    uint8_t *ptr = (uint8_t *)initrd;
    size_t this_dir_len = strlen(this_dir);

    if (this_dir_len + 1 >= 128) {
        kstatus("error", "path too long\n");

        return NULL;
    }

    bool found_dir = false;
    static char *printed[MAX_ENTRIES];
    int printed_count = 0;
    bool already_printed = false;

    static char *entries[MAX_ENTRIES];
    
    for (int i = 0; i < MAX_ENTRIES; i++) {
        entries[i] = NULL;
        printed[i] = NULL;
    };

    while (1) {
        tar_header_t *header = (tar_header_t *)ptr;

        if (header->name[0] == '\0') break;

        if (strncmp(header->magic, "ustar", 5) != 0) {
            kstatus("error", "invalid tar header magic: %c%c%c%c%c\n",
                header->magic[0], header->magic[1], header->magic[2],
                header->magic[3], header->magic[4]);
            
            break;
        }

        char *name = header->name;

        if (strncmp(name, this_dir, this_dir_len) == 0) {
            found_dir = true;

            if (strcmp(name, this_dir) == 0) {
                uint32_t size = otod(header->size);

                ptr += TAR_BLOCK_SIZE + ((size + TAR_BLOCK_SIZE - 1) & ~(TAR_BLOCK_SIZE - 1));

                continue;
            }

            char *subpath = name + this_dir_len;
            char *slash = strchr(subpath, '/');
            char entry_name[256] = {0};
            size_t len;

            if (slash) {
                len = slash - subpath + 1;

                char *before = strndup(subpath, len);

                already_printed = false;
                
                for (int i = 0; i <= printed_count; i++) {
                    if (strcmp(printed[i], before) == 0) {
                        already_printed = true;

                        break;
                    }
                }

                if (!already_printed)
                    printed[printed_count++] = before;
            } else
                len = strlen(subpath);
            
            if (already_printed) {
                uint32_t size = otod(header->size);

                already_printed = false;

                ptr += TAR_BLOCK_SIZE + ((size + TAR_BLOCK_SIZE - 1) & ~(TAR_BLOCK_SIZE - 1));

                continue;
            }

            if (len >= sizeof(entry_name)) len = sizeof(entry_name) - 1;

            strncpy(entry_name, subpath, len);
            entry_name[len] = '\0';

            if (strcmp(entry_name, "") == 0) {
                uint32_t size = otod(header->size);

                ptr += TAR_BLOCK_SIZE + ((size + TAR_BLOCK_SIZE - 1) & ~(TAR_BLOCK_SIZE - 1));

                continue;
            }

            entries[*entry_count] = strdup(entry_name);
            (*entry_count)++;
        }

        uint32_t size = otod(header->size);

        ptr += TAR_BLOCK_SIZE + ((size + TAR_BLOCK_SIZE - 1) & ~(TAR_BLOCK_SIZE - 1));
    }

    if (!found_dir)
        return NULL;
    else
        return entries;
}

void *read_file(const char *file_name, uint32_t *file_size) {
    uint8_t *ptr = (uint8_t *)initrd;

    while (1) {
        tar_header_t *header = (tar_header_t *)ptr;

        if (header->name[0] == '\0')
            return NULL;
        
        if (strncmp(header->magic, "ustar", 5) != 0)
            return NULL;
        
        if (strcmp(header->name, file_name) == 0) {
            uint32_t size = otod(header->size);

            if (file_size) *file_size = size;

            void *file_data = ptr + TAR_BLOCK_SIZE;

            return file_data;
        }

        uint32_t size = otod(header->size);

        ptr += TAR_BLOCK_SIZE + ((size + TAR_BLOCK_SIZE - 1) & ~(TAR_BLOCK_SIZE - 1));
    }
}