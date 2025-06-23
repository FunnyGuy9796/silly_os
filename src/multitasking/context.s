.globl switch_context
.type switch_context, @function

switch_context:
    cli

    movl %ebx, 0(%eax)
    movl %esi, 4(%eax)
    movl %edi, 8(%eax)
    movl %ebp, 12(%eax)
    movl %esp, 16(%eax)

    call save_eip
save_eip:
    pop %ecx
    movl %ecx, 20(%eax)

    movl 0(%edx), %ebx
    movl 4(%edx), %esi
    movl 8(%edx), %edi
    movl 12(%edx), %ebp
    movl 16(%edx), %esp
    movl 20(%edx), %ecx

    sti

    jmp *%ecx