add_library(cursor STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cursor.c
)

target_compile_options(cursor PRIVATE
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

target_include_directories(cursor
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libc/include
)

target_link_libraries(cursor PRIVATE display)