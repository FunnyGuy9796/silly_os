CC = ${HOME}/opt/cross/bin/i686-elf-gcc
LD = ${HOME}/opt/cross/bin/i686-elf-ld

LIBGCC_PATH = ${HOME}/opt/cross/lib/gcc/i686-elf/15.1.0/libgcc.a

CFLAGS = -ffreestanding -m32 -nostdlib -fno-pic -fno-pie -no-pie
LDFLAGS = -r -m elf_i386

SRC_DIR := src
BUILD_DIR := build/obj

SRC := $(shell find $(SRC_DIR) -name '*.c')

OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

all: test.ko

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test.ko: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

clean:
	rm -rf build *.o *.ko