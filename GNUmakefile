CC      := gcc
LD      := ld
OBJCOPY := objcopy

CFLAGS := \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pic \
    -no-pie \
    -std=gnu23 \
    -mno-red-zone \
    -nostdlib \
    -nostdinc \
    -nodefaultlibs \
    -O2 \
    -Wall \
    -Wextra \
    -m64 \
    -I src/include \
    -I libs/libc/include \
    -I libs/display/include \
    -I libs/cursor/include \
    -I libs/compositor/include \
    -I libs/term/include

LDFLAGS := \
    -nostdlib \
    -static \
    -T link.ld

SRC_DIR        := src
BUILD_DIR      := build
LIBS_DIR       := libs
LIBS_BUILD_DIR := libs/build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

LIBC       := $(LIBS_BUILD_DIR)/libc/liblibc.a
LIBDISPLAY := $(LIBS_BUILD_DIR)/display/libdisplay.a
LIBCURSOR      := $(LIBS_BUILD_DIR)/cursor/libcursor.a
LIBCOMPOSITOR := $(LIBS_BUILD_DIR)/compositor/libcompositor.a
LIBTERM      := $(LIBS_BUILD_DIR)/term/libterm.a
CRT        := $(LIBS_BUILD_DIR)/libc/CMakeFiles/crt.dir/__/crt/crt.asm.o

TARGET := USER

.PHONY: all clean libs

all: libs $(BUILD_DIR) $(TARGET)

libs: $(LIBS_BUILD_DIR)
	cmake --build $(LIBS_BUILD_DIR) -j$(shell nproc)

$(LIBS_BUILD_DIR):
	cmake -S $(LIBS_DIR) -B $(LIBS_BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(CRT) $(OBJS) $(LIBCURSOR) $(LIBDISPLAY) $(LIBCOMPOSITOR) $(LIBTERM) $(LIBC)
	$(LD) $(LDFLAGS) -o $@ $(CRT) $(OBJS) $(LIBCURSOR) $(LIBDISPLAY) $(LIBCOMPOSITOR) $(LIBTERM) $(LIBC)

clean:
	rm -rf $(BUILD_DIR) $(LIBS_BUILD_DIR) $(TARGET)