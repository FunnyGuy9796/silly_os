#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "../display/printf.h"
#include "../misc/byte.h"
#include "../pic/pic.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_BUFFER_SIZE 256

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_index = 0;
static int input_ready = 0;

static int shift_pressed = 0;

void keyboard_init();

void keyboard_isr();

void keyboard_read_line(char *buf, int max_len);

#endif