CC = ${HOME}/opt/cross/bin/i686-elf-gcc
LD = ${HOME}/opt/cross/bin/i686-elf-ld
OBJDUMP = ${HOME}/opt/cross/bin/i686-elf-objdump
ELFREAD = ${HOME}/opt/cross/bin/i686-elf-readelf

LIBGCC_PATH = ${HOME}/opt/cross/lib/gcc/i686-elf/15.1.0/libgcc.a

CFLAGS = -m32 -ffreestanding -O0 -g -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld --no-gc-sections

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

kernel.elf: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBGCC_PATH)

silly.iso: kernel.elf
	mkdir -p build/iso/boot/grub
	cp kernel.elf build/iso/boot/
	tar --format=ustar -cvf initrd.tar initrd
	cp initrd.tar build/iso/boot/
	cp boot/grub/grub.cfg build/iso/boot/grub/
	grub2-mkrescue -o silly.iso build/iso

run: silly.iso
	qemu-system-i386 -cdrom silly.iso -m 4G -rtc base=localtime

debug: silly.iso
	qemu-system-i386 -cdrom silly.iso -monitor stdio -d int,cpu_reset -m 4G -rtc base=localtime -no-reboot -no-shutdown

clean:
	rm -rf build *.o *.elf *.iso *.tar

dump:
	$(OBJDUMP) -D kernel.elf

read:
	${ELFREAD} -s kernel.elf