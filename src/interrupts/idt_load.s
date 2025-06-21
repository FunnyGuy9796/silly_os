.global idt_load
idt_load:
    movl 4(%esp), %eax
    lidt (%eax)
    ret