.global switch_thread
.type switch_thread, @function

switch_thread:
    cli

    movl %esp, (%eax)

    movl %edx, %esp

    sti

    ret