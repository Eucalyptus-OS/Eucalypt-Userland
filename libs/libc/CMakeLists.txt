add_library(libc STATIC
    src/stdio.c
    src/stdlib.c
    src/string.c
)

target_compile_options(libc PRIVATE
    -ffreestanding
    -fno-stack-protector
    -fno-pic
    -no-pie
    -mno-red-zone
    -nostdlib
    -O2
    -Wall
    -Wextra
    -m64
)

target_include_directories(libc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(crt OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/../crt/crt.asm)