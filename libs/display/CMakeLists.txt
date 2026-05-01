add_library(display STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display.c
)

target_compile_options(display PRIVATE
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

target_include_directories(display
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libc/include
)

target_link_libraries(display PRIVATE libc)