#ifndef SHELL_H
#define SHELL_H

#include "../display/display.h"
#include "../devices/keyboard.h"
#include "../timer/timer.h"
#include "../misc/string.h"
#include "command.h"

#define MAX_ARGS 10

void shell_init();

#endif