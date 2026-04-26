CC   := gcc
AR   := ar
NASM := nasm

CFLAGS := \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pic \
    -no-pie \
    -mno-red-zone \
    -nostdlib \
    -O2 \
    -Wall \
    -Wextra \
    -m64 \
    -I libc/include

NASMFLAGS := -f elf64

SRC_DIR   := libc
CRT_DIR   := crt
BUILD_DIR := build

C_SRCS := $(wildcard $(SRC_DIR)/*.c)
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))

.PHONY: all clean

all: $(BUILD_DIR) libc.a crt.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

libc.a: $(C_OBJS)
	$(AR) rcs $@ $^

crt.o: $(CRT_DIR)/crt.asm
	$(NASM) $(NASMFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR) libc.a crt.o
