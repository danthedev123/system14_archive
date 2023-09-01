bits 64
global _start

section .text
_start:
    MOV al, 3
    INT 0x80
    JMP $