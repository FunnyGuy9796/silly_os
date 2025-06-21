CC = ${HOME}/opt/cross/bin/i686-elf-gcc
LD = ${HOME}/opt/cross/bin/i686-elf-ld

CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

SRC_DIR := src
BUILD_DIR := build/obj

SRC := $(shell find $(SRC_DIR) -name '*.c')

OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

all: silly.iso

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

silly.iso: kernel.bin
	mkdir -p build/iso/boot/grub
	cp kernel.bin build/iso/boot/
	cp boot/grub/grub.cfg build/iso/boot/grub/
	grub2-mkrescue -o silly.iso build/iso

run: silly.iso
	qemu-system-i386 -cdrom silly.iso

clean:
	rm -rf build *.o *.bin *.iso