#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "../misc/byte.h"

#define VGA_ROWS 25
#define VGA_COLS 80

enum term_colors {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_PURPLE = 5,
    VGA_BROWN = 6,
    VGA_GRAY = 7,
    VGA_DARK_GRAY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_PURPLE = 13,
    VGA_YELLOW = 14,
    VGA_WHITE = 15
};

extern int row;
extern int col;

bool kascii(const char c);

void kcolor(int bg_color, int fg_color);

void kclear();

void kputc(const char c, int c_row, int c_col);

void kprint(const char *message);

#endif