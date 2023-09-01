#include "../util/print.h"

struct SyscallRegisters
{
    //uint64_t rsp;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    
    // Info pushed by CPU, not us
    uint64_t rip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
}__attribute__((packed));

void SyscallHandler(struct SyscallRegisters*)
{
    printf("Hello from a syscall!\n");
}
