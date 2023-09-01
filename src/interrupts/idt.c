/*
    * idt.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements the Interrupt Descriptor Table (IDT) and sets up exception handling and IRQs.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include "idt.h"
#include "../util/memutil.h"
#include <stdbool.h>
#include "../mm/allocator/allocator.h"
#include "../util/ioports.h"
#include "../drivers/keyboard.h"
#include "../system/panic.h"
#include "../drivers/pit.h"
#include "../util/print.h"
#include "../multitasking/scheduler.h"
#include "../drivers/apic/apic.h"
#include "../drivers/rtc/rtc.h"
#include "../mm/paging/paging.h"

struct InterruptDescriptor idt[256] = {0}; // 256 IDT entries
struct InterruptDescriptorTablePtr idtr;

void AddIDTEntry(struct InterruptDescriptor idt[], void* offset, size_t index, uint8_t GATE_TYPE)
{
    /* encode offset */
    idt[index].offset_1 = (uint16_t)((uint64_t)offset & 0x000000000000ffff);
    idt[index].offset_2 = (uint16_t)(((uint64_t)offset & 0x00000000ffff0000) >> 16);
    idt[index].offset_3 = (uint32_t)(((uint64_t)offset & 0xffffffff00000000) >> 32);

    /* set misc options */
    idt[index].types_attributes = GATE_TYPE;
    idt[index].ist = 0;
    idt[index].selector = 0x08; // selector -> GDT code segment selector. the selector for the CS in the gdt (see gdt.c) is 0x08.
    idt[index].null = 0;
}

// PIC remap code thanks to osdev.org (https://wiki.osdev.org/8259_PIC)
void RemapPIC(int offset1, int offset2)
{
    unsigned char a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

/* start interrupt routines */
/* ------------- */
__attribute__((interrupt)) void DivZeroHandler(void*)
{
    CommonExceptionHandler("Division by zero");   
}

/* NMI Interrupt unimplemented */

__attribute__((interrupt)) void BreakpointHandler(void*)
{
    printf("Breakpoint reached!\n");
}

__attribute__((interrupt)) void OverflowHandler(void*)
{
    CommonExceptionHandler("Overflow exception");
}

__attribute__((interrupt)) void BoundRageExceededHandler(void*)
{
    CommonExceptionHandler("Bound range exceeded");
}

__attribute__((interrupt)) void InvalidOpcodeHandler(void*)
{
    CommonExceptionHandler("Invalid opcode");
}

__attribute__((interrupt)) void DeviceNotAvailableHandler(void*)
{
    CommonExceptionHandler("Device not available");
}

__attribute__((interrupt)) void DFHandler(void*)
{
    CommonExceptionHandler("Double fault");
}

__attribute__((interrupt)) void GPFaultHandler(void*)
{
    CommonExceptionHandler("General protection fault");
}

__attribute__((interrupt)) void InvalidTSSHandler(void*)
{
    CommonExceptionHandler("Invalid TSS");
}

__attribute__((interrupt)) void PageFaultHandler(void*)
{
    CommonExceptionHandler("Page fault");
}

__attribute__((interrupt)) void KeyboardHandler(void* )
{
    LoadKernelPML4();

    int sc = inb(0x60);

    KeyboardDriver(sc);

    LAPIC_EOI();
}

/* ------------- */
/* end interrupt routines */

extern void TimerStub();
extern void SyscallStub();
extern void DisablePIC();

void InitializeIDT()
{
    memset(idt, 0, sizeof(struct InterruptDescriptor*) * 256);

    idtr.limit = 0x0fff;
    idtr.ptr = (uint64_t)&idt;

    AddIDTEntry(idt, &DivZeroHandler, INTERRUPT_DIV_BY_ZERO, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &OverflowHandler, INTERRUPT_OVERFLOW, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &BoundRageExceededHandler, INTERRUPT_BOUND_RAGE_EXCEEDED, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &InvalidOpcodeHandler, INTERRUPT_INVALID_OPCODE, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &DeviceNotAvailableHandler, INTERRUPT_DEVICE_NOT_AVAILABLE, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &DFHandler, INTERRUPT_DOUBLE_FAULT, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &InvalidTSSHandler, INTERRUPT_INVALID_TSS, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &GPFaultHandler, INTERRUPT_GP_FAULT, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &PageFaultHandler, INTERRUPT_PAGE_FAULT, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &KeyboardHandler, IRQ(IRQ_KEYBOARD), IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &BreakpointHandler, INTERRUPT_BREAKPOINT, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &SyscallStub, 0x80, IDT_GATE_INTERRUPT);
    AddIDTEntry(idt, &TimerStub, IRQ(IRQ_RTC), IDT_GATE_INTERRUPT);
    
    asm ("lidt %0" : : "m" (idtr));

    RemapPIC(0x20, 0x28);
    DisablePIC();

    InitializeAPIC();

    IOAPICRegisterIRQ(0x28, 8);
    InitializeRTC();
    IOAPICRegisterIRQ(0x21, 1); // The keyboard is on pin 1

    asm ("sti"); // Renable interrupts
}
