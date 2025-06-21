#include "display.h"

volatile char *video = (volatile char*)0xb8000;
int color = 0x0f;

int row = 0;
int col = 0;

void kcursor(int row, int col) {
    uint16_t pos = row * VGA_COLS + col;

    outb(0x3d4, 0x0e);
    outb(0x3d5, (pos >> 8) & 0xff);

    outb(0x3d4, 0x0f);
    outb(0x3d5, pos & 0xff);
}

bool kascii(const char c) {
    bool is_ascii = false;

    switch (c) {
        case '\n': {
            row++;
            col = 0;
            is_ascii = true;

            break;
        }

        case '\t': {
            col += 4;
            is_ascii = true;

            break;
        }

        case '\b': {
            col--;
            is_ascii = true;

            break;
        }
    }

    kcursor(row, col);

    return is_ascii;
}

void kcolor(int bg_color, int fg_color) {
    color = (fg_color << 4) | (bg_color & 0x0f);
}

void kclear() {
    for (int i = 0; i < VGA_ROWS; i++) {
        for (int j = 0; j < VGA_COLS; j++) {
            video[i * 2] = ' ';
            video[i * 2 + 1] = 0x00;
        }
    }
}

void kputc(const char c, int c_row, int c_col) {
    int offset = (c_row * VGA_COLS + c_col) * 2;

    if (kascii(c))
        return;

    video[offset] = c;
    video[offset + 1] = color;
    col++;

    if (col >= VGA_COLS)
        row++;

    kcursor(row, col);
}

void kprint(const char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        kputc(message[i], row, col);
    }
}