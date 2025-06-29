#include "keyboard.h"

char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';', '\'', '`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0,
};

char scancode_to_ascii_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0, 'A','S','D','F','G','H','J','K','L',':', '"', '~', 0,
    '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0, ' ', 0,
};

void keyboard_init() {
    uint8_t mask = inb(PIC1_DATA);
    mask &= ~(1 << KEYBOARD_IRQ);
    
    outb(PIC1_DATA, mask);

    kstatus("debug", "keyboard initialized\n");
}

void keyboard_callback() {
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7f;

        if (released == 0x2a || released == 0x36)
            shift_pressed = 0;

        outb(0x20, 0x20);

        return;
    }

    if (scancode == 0x2a || scancode == 0x36) {
        shift_pressed = 1;

        outb(0x20, 0x20);

        return;
    }

    char c = shift_pressed ? scancode_to_ascii_shift[scancode] : scancode_to_ascii[scancode];

    if (c) {
        if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                kprintf("\b \b");
            }
        } else if (c == '\n') {
            keyboard_buffer[buffer_index++] = '\0';
            input_ready = 1;
        } else if (buffer_index < KEYBOARD_BUFFER_SIZE - 1) {
            keyboard_buffer[buffer_index++] = c;
            kprintf("%c", c);
        }
    }

    outb(0x20, 0x20);
}

void keyboard_read_line(char *buf, int max_len) {
    DISABLE_INTERRUPTS();
    buffer_index = 0;
    input_ready = 0;
    ENABLE_INTERRUPTS();

    while (!input_ready)
        __asm__ volatile ("hlt");
    
    DISABLE_INTERRUPTS();
    
    for (int i = 0; i < buffer_index && i < max_len - 1; i++)
        buf[i] = keyboard_buffer[i];
    
    buf[buffer_index < max_len ? buffer_index : max_len - 1] = '\0';

    memset((void *)keyboard_buffer, 0, KEYBOARD_BUFFER_SIZE);

    ENABLE_INTERRUPTS();
}