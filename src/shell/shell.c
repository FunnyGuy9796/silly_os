#include "shell.h"

void interpret_command(char *input) {
    kprintf("\n");

    char *delim = " ";
    char *argv[MAX_ARGS];
    int argc = 0;

    char *token = strtok(input, delim);

    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, delim);
    }

    if (argc == 0)
        return;
    
    if (strcmp(argv[0], "help") == 0)
        cmd_help(argc, argv);
    else if (strcmp(argv[0], "echo") == 0)
        cmd_echo(argc, argv);
    else if (strcmp(argv[0], "date") == 0)
        cmd_date(argc, argv);
    else if (strcmp(argv[0], "clear") == 0)
        cmd_clear(argc, argv);
    else if (strcmp(argv[0], "mem") == 0)
        cmd_mem(argc, argv);
    else
        kprintf("silly_shell: command '%s' not found...\n", argv[0]);
}

void shell_init() {
    kstatus("debug", "silly_shell initialized\n\n");
    
    char input[128];

    while (1) {
        kprompt("user@silly_os");
        keyboard_read_line(input, sizeof(input));
        interpret_command(input);

        __asm__ volatile("hlt");
    }
}