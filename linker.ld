ENTRY(start)

SECTIONS {
    . = 1M;

    _start = .;

    .multiboot : {
        KEEP(*(.multiboot))
    }

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : { *(.data*) }
    .bss : {
        *(.bss*)
        *(COMMON)
    }

    _end = .;
}