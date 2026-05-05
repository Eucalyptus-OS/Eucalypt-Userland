add_library(compositor STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compositor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/font_renderer.c
)

target_compile_options(compositor PRIVATE
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

target_include_directories(compositor
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libc/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../display/include
)

target_link_libraries(compositor PRIVATE libc display)