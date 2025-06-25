#ifndef COMMAND_H
#define COMMAND_H

#include "../display/printf.h"
#include "../timer/timer.h"
#include "../misc/day.h"
#include "../memory/pmm.h"

void cmd_help(int argc, char **argv);

void cmd_echo(int argc, char **argv);

void cmd_date(int argc, char **argv);

void cmd_clear(int argc, char **argv);

void cmd_mem(int argc, char **argv);

#endif