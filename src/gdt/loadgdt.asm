bits 64
section .data
ALIGN 16

section .text
global loadgdt
loadgdt:
    CLI ; Clear interrupts

    LGDT [RDI] ; rdi=1st arg, gdtr ptr

    ; Load DS (Data Segment)
    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    MOV SS, AX
    
    ; Do the same for the code Segment
    ; We can't just do `MOV CS, 0x08`, we instead must use a far return in x86_64.

    POP RDI
    MOV RAX, 0x08
    PUSH RAX
    PUSH RDI
    RETFQ
    
    ; Interrupts are later re-enabled from interrupts code

