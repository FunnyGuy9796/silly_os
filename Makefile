CC = ${HOME}/opt/cross/bin/i686-elf-gcc
LD = ${HOME}/opt/cross/bin/i686-elf-ld

CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

SRC_DIR := src
BUILD_DIR := build/obj

C_SRC := $(shell find $(SRC_DIR) -name '*.c')
ASM_S_SRC := $(shell find $(SRC_DIR) -name '*.s')
ASM_S_CAP_SRC := $(shell find $(SRC_DIR) -name '*.S')

C_OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))
ASM_S_OBJ := $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(ASM_S_SRC))
ASM_S_CAP_OBJ := $(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%.o,$(ASM_S_CAP_SRC))

ASM_OBJ := $(ASM_S_OBJ) $(ASM_S_CAP_OBJ)
OBJ := $(C_OBJ) $(ASM_OBJ)

all: silly.iso

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

silly.iso: kernel.bin
	mkdir -p build/iso/boot/grub
	cp kernel.bin build/iso/boot/
	cp boot/grub/grub.cfg build/iso/boot/grub/
	grub2-mkrescue -o silly.iso build/iso

run: silly.iso
	qemu-system-i386 -cdrom silly.iso -m 4G -rtc base=localtime

clean:
	rm -rf build *.o *.bin *.iso