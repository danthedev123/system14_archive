#pragma once
#include <stdint.h>

#define IDT_GATE_INTERRUPT 0x8E
#define IDT_GATE_TRAP 0x8F

#define INTERRUPT_DIV_BY_ZERO 1
#define INTERRUPT_NMI 2
#define INTERRUPT_BREAKPOINT 3
#define INTERRUPT_OVERFLOW 4
#define INTERRUPT_BOUND_RAGE_EXCEEDED 5
#define INTERRUPT_INVALID_OPCODE 6
#define INTERRUPT_DEVICE_NOT_AVAILABLE 7
#define INTERRUPT_DOUBLE_FAULT 8
#define INTERRUPT_COPROCESSOR_SEGMENT 9
#define INTERRUPT_INVALID_TSS 10
#define INTERRUPT_GP_FAULT 13
#define INTERRUPT_PAGE_FAULT 14

#define IRQ_KEYBOARD 1
#define IRQ_PIT 0
#define IRQ_RTC 8

#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_DATA (PIC1+1)
#define PIC2_DATA (PIC2+1)
#define PIC1_COMMAND PIC1
#define PIC2_COMMAND PIC2

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10

#define ICW4_8086 0x01

#define PIC_EOI 0x20

#define IRQ(n) 32 + (n)

struct Registers
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
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

}__attribute__((packed)); // Make sure that the structure doesn't get incorrectly packed
                          // or optimized by the compiler

struct InterruptDescriptor
{
    uint16_t offset_1;
    uint16_t selector;
    uint8_t ist;
    uint8_t types_attributes;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t null;
}__attribute__((packed))
__attribute__((aligned(16)));

struct InterruptDescriptorTablePtr
{
    uint16_t limit;
    uint64_t ptr;
}__attribute__((packed))
__attribute__((aligned(16)));

void InitializeIDT();

void Signal_EOI();
void Signal_EOI_8();