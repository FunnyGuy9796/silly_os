#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "../display/printf.h"
#include "../misc/byte.h"
#include "../misc/mem.h"
#include "../pic/pic.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_BUFFER_SIZE 256

#define DISABLE_INTERRUPTS() __asm__ volatile("cli")
#define ENABLE_INTERRUPTS()  __asm__ volatile("sti")

static volatile char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int buffer_index = 0;
static volatile int input_ready = 0;

static volatile int shift_pressed = 0;

void keyboard_init();

void keyboard_callback();

void keyboard_read_line(char *buf, int max_len);

#endif