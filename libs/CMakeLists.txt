cmake_minimum_required(VERSION 3.20)
project(EucalyptUserland C ASM_NASM)

set(CMAKE_C_STANDARD 23)

add_subdirectory(libc)
add_subdirectory(display)
add_subdirectory(wm)
add_subdirectory(cursor)
add_subdirectory(compositor)
