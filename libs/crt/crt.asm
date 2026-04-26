bits 64

section .text
extern main
global _start

_start:
    xor rbp, rbp
    call main
hang:
    hlt
    jmp hang
