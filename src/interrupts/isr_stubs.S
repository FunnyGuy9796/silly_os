.global isr_dummy
isr_dummy:
    iret

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr28
.global isr29
.global isr30
.global isr32
.global isr33

.section .text

.macro ISR_NO_ERR_CODE num
isr\num:
    cli
    pushl $0
    pushl $\num
    jmp isr_common_stub
.endm

.macro ISR_ERR_CODE num
isr\num:
    cli
    pushl $\num
    jmp isr_common_stub
.endm

ISR_NO_ERR_CODE 0
ISR_NO_ERR_CODE 1
ISR_NO_ERR_CODE 2
ISR_NO_ERR_CODE 3
ISR_NO_ERR_CODE 4
ISR_NO_ERR_CODE 5
ISR_NO_ERR_CODE 6
ISR_NO_ERR_CODE 7
ISR_NO_ERR_CODE 16
ISR_NO_ERR_CODE 18
ISR_NO_ERR_CODE 19
ISR_NO_ERR_CODE 20
ISR_NO_ERR_CODE 28
ISR_NO_ERR_CODE 32
ISR_NO_ERR_CODE 33
ISR_ERR_CODE 8
ISR_ERR_CODE 10
ISR_ERR_CODE 11
ISR_ERR_CODE 12
ISR_ERR_CODE 13
ISR_ERR_CODE 14
ISR_ERR_CODE 17
ISR_ERR_CODE 21
ISR_ERR_CODE 29
ISR_ERR_CODE 30

.global isr_common_stub
isr_common_stub:
    pushal
    pushl %ds
    
    mov $0x10, %ax
    mov %ax, %ds
    
    pushl %esp
    call isr_handler
    addl $4, %esp
    
    popl %ds
    popal
    addl $8, %esp
    sti
    iret