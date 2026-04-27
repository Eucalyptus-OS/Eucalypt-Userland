CC      := gcc
LD      := ld
OBJCOPY := objcopy

CFLAGS := \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pic \
    -no-pie \
    -std=c23 \
    -mno-red-zone \
    -nostdlib \
    -nostdinc \
    -nodefaultlibs \
    -O2 \
    -Wall \
    -Wextra \
    -m64 \
    -I src/include \
    -I libs/libc/include

LDFLAGS := \
    -nostdlib \
    -static \
    -T link.ld

SRC_DIR   := src
BUILD_DIR := build
LIBS_DIR  := libs

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

LIBC := $(LIBS_DIR)/libc.a
CRT  := $(LIBS_DIR)/crt.o

TARGET := USER

.PHONY: all clean libs

all: libs $(BUILD_DIR) $(TARGET)

libs:
	$(MAKE) -C $(LIBS_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(CRT) $(OBJS) $(LIBC)
	$(LD) $(LDFLAGS) -o $@ $(CRT) $(OBJS) $(LIBC)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	$(MAKE) -C $(LIBS_DIR) clean
