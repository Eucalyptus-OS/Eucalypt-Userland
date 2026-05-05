add_library(term STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/term.c
)

target_compile_options(term PRIVATE
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

target_include_directories(term
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libc/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../compositor/include
)

target_link_libraries(term PRIVATE libc display)