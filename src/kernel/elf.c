#include "elf.h"

uint32_t elf_num;
uint32_t elf_size;
uint32_t elf_addr;
uint32_t elf_shndx;

void elf_init(multiboot_info_t *mb_info) {
    elf_num = mb_info->syms.elf_sec.num;
    elf_size = mb_info->syms.elf_sec.size;
    elf_addr = mb_info->syms.elf_sec.addr;
    elf_shndx = mb_info->syms.elf_sec.shndx;

    kstatus("debug", "elf information located at address: 0x%x\n", elf_addr);
}

const char *get_symbol(uint32_t addr, uint32_t *offset) {
    Elf32_Shdr *shdrs = (Elf32_Shdr *)elf_addr;
    Elf32_Shdr *symtab_shdr = NULL;
    Elf32_Shdr *strtab_shdr = NULL;

    for (uint16_t i = 0; i < elf_num; i++) {
        if (shdrs[i].sh_type == SHT_SYMTAB) {
            symtab_shdr = &shdrs[i];

            if (symtab_shdr->sh_link < elf_num)
                strtab_shdr = &shdrs[symtab_shdr->sh_link];

            break;
        }
    }

    if (!symtab_shdr || !strtab_shdr) return NULL;

    Elf32_Sym *symbols = (Elf32_Sym *)symtab_shdr->sh_addr;
    const char *strings = (const char *)strtab_shdr->sh_addr;
    uint32_t num_symbols = symtab_shdr->sh_size / sizeof(Elf32_Sym);

    for (uint32_t i = 0; i < num_symbols; i++) {
        Elf32_Sym *sym = &symbols[i];

        if (sym->st_size == 0) continue;

        if (addr >= sym->st_value && addr < (sym->st_value + sym->st_size)) {
            if (offset)
                *offset = addr - sym->st_value;

            return strings + sym->st_name;
        }
    }

    return NULL;
}