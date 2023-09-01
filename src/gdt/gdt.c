#include "gdt.h"
#include <stddef.h>

struct GlobalDescriptorTable globalDescriptorTable =
{
    // Null segment, 0x0
    {
        0
    },
    // Kernel mode code segment, 0x08
    {
        // base is ignore in 64-bit long mode
        .base_low = 0,
        .base_middle = 0,
        .base_high = 0,

        .limit = 0xFF,
        .access = 0x9A,
        .granularity = 0xA0,
    },
    // kernel mode data segment, 0x10
    {
        .base_low = 0,
        .base_middle = 0,
        .base_high = 0,

        .limit = 0xFF,
        .access = 0x92,
        .granularity = 0xC0,
    },
    // user mode code segment, 0x18
    {
        .base_low = 0,
        .base_middle = 0,
        .base_high = 0,

        .limit = 0,
        .access = 0xFA,
        .granularity = 0xA0,
    },
    // user mode data segment, 0x20
    {
        .base_low = 0,
        .base_middle = 0,
        .base_high = 0,

        .limit = 0,
        .access = 0xF2,
        .granularity = 0xC0,
    },
};

struct GlobalDescriptorTablePtr gdtPtr =
{
    .size = sizeof(struct GlobalDescriptorTable) - 1,
    .addr = (uint64_t)&globalDescriptorTable
};

void InitializeGDT()
{
    loadgdt(&gdtPtr);
}