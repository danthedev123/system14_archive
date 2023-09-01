section .text
;extern PITTickHandler
extern TaskSwitch
extern LAPIC_EOI
extern SyscallHandler
extern LoadKernelPML4
extern _TaskSwitch_Stage2
global TimerStub
global SyscallStub
global DisablePIC
; For scheduling we need to save register states, so this stub
; pushes them to the stack, and then provides a pointer to the stack
; as the first parameter to the interrupt handler.

TimerStub:
    ;CLD
    CALL LoadKernelPML4

    ; Once the exception handler is called, certain values are pushed to the stack
    ; such as the CS, IP, SS etc
    
    PUSH rax
    PUSH rbx
    PUSH rcx
    PUSH rdx
    PUSH rsi
    PUSH rdi
    PUSH rbp
   ; PUSH rsp
    PUSH r8
    PUSH r9
    PUSH r10
    PUSH r11
    PUSH r12
    PUSH r13
    PUSH r14
    PUSH r15

    MOV rdi, rsp ; RDI (the first parameter of the function call) now contains the stack pointer
    
    CALL TaskSwitch
    CALL LAPIC_EOI
    CALL _TaskSwitch_Stage2

    POP r15
    POP r14
    POP r13
    POP r12
    POP r11
    POP r10
    POP r9
    POP r8
   ;  POP rsp
    POP rbp
    POP rdi
    POP rsi
    POP rdx
    POP rcx
    POP rbx
    POP rax

    IRETQ

SyscallStub:
    CALL LoadKernelPML4

    PUSH rax
    PUSH rbx
    PUSH rcx
    PUSH rdx
    PUSH rsi
    PUSH rdi
    PUSH rbp

    MOV rdi, rsp
    CALL SyscallHandler
    CALL LAPIC_EOI

    POP rbp
    POP rdi
    POP rsi
    POP rdx
    POP rcx
    POP rbx
    POP rax

    IRETQ

DisablePIC:
    MOV al, 0xff
    OUT 0xa1, al
    OUT 0x21, al
    
    RET