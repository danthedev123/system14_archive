#pragma once
#include <stdint.h>

struct SegmentDescriptor
{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));

struct GlobalDescriptorTable
{
    struct SegmentDescriptor null;
    struct SegmentDescriptor kernelCode;
    struct SegmentDescriptor kernelData;
    struct SegmentDescriptor userCode;
    struct SegmentDescriptor userData;
    /* struct SegmentDescriptor tss */
}__attribute__((packed));

struct GlobalDescriptorTablePtr {
    uint16_t size;
    uint64_t addr;
}__attribute__((packed));

extern void loadgdt(struct GlobalDescriptorTablePtr* ptr);
void InitializeGDT();