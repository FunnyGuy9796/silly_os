#include "elf.h"

uint32_t elf_num;
uint32_t elf_size;
uint32_t elf_addr;
uint32_t elf_shndx;

int called = 0;

ksymbol_t kernel_symbols[] = {
    { "kprintf", kprintf },
    { "kstatus", kstatus },
    { "kpanic", kpanic },
    { "ksnprintf", ksnprintf },
    { "kmalloc", kmalloc },
    { "kfree", kfree },
    { "krealloc", krealloc },
    { "list_tar", list_tar },
    { "read_file", read_file },
    { NULL, NULL }
};

void elf_init(multiboot_info_t *mb_info) {
    elf_num = mb_info->syms.elf_sec.num;
    elf_size = mb_info->syms.elf_sec.size;
    elf_addr = mb_info->syms.elf_sec.addr;
    elf_shndx = mb_info->syms.elf_sec.shndx;

    kstatus("debug", "elf information located at address: 0x%x\n", elf_addr);
}

const char *get_symbol(uint32_t addr) {
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

        if (addr >= sym->st_value && addr < (sym->st_value + sym->st_size)) return strings + sym->st_name;
    }

    return NULL;
}

void *get_symbol_addr(const char *name) {
    for (int i = 0; kernel_symbols[i].name != NULL; i++)
        if (strcmp(kernel_symbols[i].name, name) == 0)
            return kernel_symbols[i].addr;

    return NULL;
}

int parse_module(void *module_base, Elf32_Ehdr **ehdr_out, Elf32_Shdr **shdrs_out, const char **shstrtab_out) {
    if (!module_base) return 1;

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)module_base;

    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        kstatus("error", "invalid elf magic\n");

        return 1;
    }

    if (ehdr->e_ident[EI_CLASS] != 1 || ehdr->e_ident[EI_DATA] != 1) {
        kstatus("error", "unsupported elf class or data encoding\n");

        return 1;
    }

    if (ehdr->e_type != 1) {
        kstatus("error", "unsupported elf type (expected ELF_REL)\n");

        return 1;
    }

    Elf32_Shdr *shdrs = (Elf32_Shdr *)((uint8_t *)module_base + ehdr->e_shoff);

    if (ehdr->e_shstrndx == SHN_UNDEF) {
        kstatus("error", "no section header string table\n");

        return 1;
    }

    Elf32_Shdr *shstr_shdr = &shdrs[ehdr->e_shstrndx];
    const char *shstrtab = (const char *)((uint8_t *)module_base + shstr_shdr->sh_offset);

    if (ehdr_out) *ehdr_out = ehdr;
    if (shdrs_out) *shdrs_out = shdrs;
    if (shstrtab_out) *shstrtab_out = shstrtab;

    return 0;
}