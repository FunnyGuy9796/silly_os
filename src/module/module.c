#include "module.h"

int load_module(void *module_base) {
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdrs;
    const char *shstrtab;

    if (parse_module(module_base, &ehdr, &shdrs, &shstrtab) != 0) {
        kstatus("error", "load_module(): failed to parse elf module\n");

        return 1;
    }

    void **section_addrs = kmalloc(sizeof(void *) * ehdr->e_shnum, 0);

    if (!section_addrs) {
        kstatus("error", "load_module(): failed to allocate section address table\n");

        return 1;
    }

    for (uint16_t i = 0; i < ehdr->e_shnum; i++) {
        Elf32_Shdr *shdr = &shdrs[i];
        const char *sec_name = shstrtab + shdr->sh_name;
        void *sec_addr = NULL;

        if (shdr->sh_type == SHT_PROGBITS) {
            sec_addr = kmalloc(shdr->sh_size, 0);

            if (!sec_addr) {
                kstatus("error", "load_module(): failed to allocate sections %s\n", sec_name);

                return 1;
            }

            memcpy(sec_addr, (uint8_t *)module_base + shdr->sh_offset, shdr->sh_size);
        } else if (shdr->sh_type == SHT_NOBITS) {
            sec_addr = kmalloc(shdr->sh_size, 0);

            if (!sec_addr) {
                kstatus("error", "load_module(): failed to allocate sections %s\n", sec_name);

                return 1;
            }

            memset(sec_addr, 0, shdr->sh_size);
        }

        section_addrs[i] = sec_addr;
    }

    for (uint16_t i = 0; i < ehdr->e_shnum; i++) {
        Elf32_Shdr *rel_shdr = &shdrs[i];

        if (rel_shdr->sh_type != SHT_REL) continue;

        Elf32_Shdr *target_shdr = &shdrs[rel_shdr->sh_info];
        void *target_section = section_addrs[rel_shdr->sh_info];
        
        Elf32_Shdr *symtab_shdr = &shdrs[rel_shdr->sh_link];
        Elf32_Sym *symbols = (Elf32_Sym *)((uint8_t *)module_base + symtab_shdr->sh_offset);
        const char *strtab = (const char *)((uint8_t *)module_base + shdrs[symtab_shdr->sh_link].sh_offset);

        Elf32_Rel *rels = (Elf32_Rel *)((uint8_t *)module_base + rel_shdr->sh_offset);
        uint32_t num_rels = rel_shdr->sh_size / sizeof(Elf32_Rel);

        for (uint32_t r = 0; r < num_rels; r++) {
            Elf32_Rel *rel = &rels[r];
            uint32_t rel_type = ELF32_R_TYPE(rel->r_info);
            uint32_t *patch_addr = (uint32_t *)((uint8_t *)target_section + rel->r_offset);
            uint32_t sym_idx = ELF32_R_SYM(rel->r_info);
            Elf32_Sym *sym = &symbols[sym_idx];

            void *sym_addr = NULL;

            if (sym->st_shndx == SHN_UNDEF) {
                const char *sym_name = strtab + sym->st_name;

                sym_addr = get_symbol_addr(sym_name);

                if (!sym_addr) return 1;
            } else
                sym_addr = (uint8_t *)section_addrs[sym->st_shndx] + sym->st_value;

            if (rel_type == R_386_32) {
                uint32_t a = *patch_addr;
                uint32_t s = (uint32_t)sym_addr;

                *patch_addr = s + a;
            } else if (rel_type == R_386_PC32) {
                uint32_t a = *patch_addr;
                uint32_t s = (uint32_t)sym_addr;
                uint32_t p = (uint32_t)patch_addr;

                *patch_addr = (s + a) - p;
            } else
                kstatus("warn", "load_module(): unhandled relocation type %d\n", rel_type);
        }
    }

    for (uint16_t i = 0; i < ehdr->e_shnum; i++) {
        Elf32_Shdr *shdr = &shdrs[i];

        if (shdr->sh_type != SHT_SYMTAB) continue;

        Elf32_Sym *symbols = (Elf32_Sym *)((uint8_t *)module_base + shdr->sh_offset);
        uint32_t num_syms = shdr->sh_size / sizeof(Elf32_Sym);
        const char *strtab = (const char *)((uint8_t *)module_base + shdrs[shdr->sh_link].sh_offset);

        for (uint32_t s = 0; s < num_syms; s++) {
            Elf32_Sym *sym = &symbols[s];

            if (sym->st_name == 0) continue;

            const char *sym_name = strtab + sym->st_name;

            if (strcmp(sym_name, "module_init") == 0) {
                void (*init_func)(void) = (void (*)(void))((uint8_t *)section_addrs[sym->st_shndx] + sym->st_value);

                uint8_t *code = (uint8_t *)init_func;

                for (int i = 0; i < 16; i++) {
                    if (code[i] == 0xe8) {
                        int32_t rel_offset = *(int32_t *)&code[i+1];
                        void *target = &code[i + 5] + rel_offset;
                    }
                }

                init_func();

                return 0;
            }
        }
    }

    kstatus("warn", "no module_init symbol found\n");

    return 0;
}