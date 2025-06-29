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
    color = (bg_color << 4) | (fg_color & 0x0f);
}

void kclear() {
    for (int i = 0; i < VGA_ROWS; i++) {
        for (int j = 0; j < VGA_COLS; j++) {
            int offset = (i * VGA_COLS + j) * 2;
            video[offset] = ' ';
            video[offset + 1] = color;
        }
    }
    row = 0;
    col = 0;
}

void kscroll() {
    if (row < VGA_ROWS)
        return;
    
    int line_size = VGA_COLS * 2;

    memcpy((void*)video, (void*)(video + line_size), (VGA_ROWS - 1) * line_size);

    int last_line_offset = (VGA_ROWS - 1) * line_size;

    for (int i = 0; i < VGA_COLS; i++) {
        video[last_line_offset + i * 2] = ' ';
        video[last_line_offset + i * 2 + 1] = color;
    }

    row = VGA_ROWS - 1;
}

void kputc(const char c) {
    if (kascii(c))
        return;

    if (col >= VGA_COLS) {
        col = 0;
        row++;
    }

    if (row >= VGA_ROWS)
        kscroll();
    
    int offset = (row * VGA_COLS + col) * 2;

    video[offset] = c;
    video[offset + 1] = color;
    col++;

    kcursor(row, col);
}

void kprint(const char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        kputc(message[i]);
    }
}

void buf_kputc(char **buf_ptr, size_t *remaining, char c, int *count) {
    if (*remaining > 1) {
        **buf_ptr = c;
        (*buf_ptr)++;
        (*remaining)--;
    }

    (*count)++;
}