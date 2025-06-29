#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"
#include "../display/printf.h"

#define EI_MAG0       0 /* 0x7F */
#define EI_MAG1       1 /* 'E'  */
#define EI_MAG2       2 /* 'L'  */
#define EI_MAG3       3 /* 'F'  */
#define EI_CLASS      4 /* File class */
#define EI_DATA       5 /* Data encoding */
#define EI_VERSION    6 /* File version */
#define EI_OSABI      7 /* OS/ABI identification */
#define EI_ABIVERSION 8 /* ABI version */
#define EI_PAD        9 /* Start of padding bytes */
#define EI_NIDENT     16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3

#define ELF32_ST_BIND(info) ((info) >> 4)
#define ELF32_ST_TYPE(info) ((info) & 0xf)

typedef struct {
    unsigned char e_ident[16]; /* ELF identification */
    uint16_t e_type;           /* Object file type */
    uint16_t e_machine;        /* Machine type */
    uint32_t e_version;        /* Object file version */
    uint32_t e_entry;          /* Entry point address */
    uint32_t e_phoff;          /* Program header offset */
    uint32_t e_shoff;          /* Section header offset */
    uint32_t e_flags;          /* Processor-specific flags */
    uint16_t e_ehsize;         /* ELF header size */
    uint16_t e_phentsize;      /* Size of program header entry */
    uint16_t e_phnum;          /* Number of program header entries */
    uint16_t e_shentsize;      /* Size of section header entry */
    uint16_t e_shnum;          /* Number of section header entries */
    uint16_t e_shstrndx;       /* Section name string table index */
} Elf32_Ehdr;

typedef struct {
    uint32_t sh_name;      /* Section name (string tbl index) */
    uint32_t sh_type;      /* Section type */
    uint32_t sh_flags;     /* Section flags */
    uint32_t sh_addr;      /* Section virtual addr at execution */
    uint32_t sh_offset;    /* Section file offset */
    uint32_t sh_size;      /* Section size in bytes */
    uint32_t sh_link;      /* Link to another section */
    uint32_t sh_info;      /* Additional section information */
    uint32_t sh_addralign; /* Section alignment */
    uint32_t sh_entsize;   /* Entry size if section holds table */
} Elf32_Shdr;

typedef struct {
    uint32_t st_name;  /* Symbol name (string table index) */
    uint32_t st_value; /* Symbol value (address) */
    uint32_t st_size;  /* Size of the symbol */
    unsigned char st_info; /* Type and binding attributes */
    unsigned char st_other;/* Reserved (0) */
    uint16_t st_shndx; /* Section index */
} Elf32_Sym;

extern uint32_t elf_num;
extern uint32_t elf_size;
extern uint32_t elf_addr;
extern uint32_t elf_shndx;

void elf_init(multiboot_info_t *mb_info);

const char *get_symbol(uint32_t addr, uint32_t *offset);

#endif