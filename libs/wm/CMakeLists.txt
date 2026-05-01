add_library(wm STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/window.c
)

target_compile_options(wm PRIVATE
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

target_include_directories(wm
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libc/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cursor/include
)

target_link_libraries(wm
    PUBLIC  cursor
    PRIVATE libc display
)