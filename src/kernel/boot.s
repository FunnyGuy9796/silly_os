.global start
start:
    cli
    mov $0x00201000, %esp

    push %ebx
    push %eax

    call kmain

.hang:
    hlt
    jmp .hang