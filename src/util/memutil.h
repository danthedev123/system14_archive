/*
    * memutil.h
    * 
    * ABSTRACT:
    * 
    *   -> Implements standard memory utility functions.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#pragma once
#include <stddef.h>
#include <stdint.h>

#define ALIGN_UP(value, boundary) (((value) + ((boundary) - 1)) / (boundary) * (boundary))
#define ALIGN_DOWN(value, boundary)  ((value) & (~((boundary) - 1)))

void memset(void* memtoset, int value, size_t buffsize);
void memcpy(void* dest, void* src, size_t n);
uintptr_t KVaddrToPaddr(uintptr_t vaddr, uintptr_t kernelVirtStart, uintptr_t kernelPhysStart);

static inline void mmio_write64(void* target, uint64_t value)
{
    *(volatile uint64_t*)target = value;
}

static inline void mmio_write32(void* target, uint32_t value)
{
    *(volatile uint32_t*)target = value;
}

static inline void mmio_write16(void* target, uint16_t value)
{
    *(volatile uint16_t*)target = value;
}

static inline void mmio_write8(void* target, uint8_t value)
{
    *(volatile uint8_t*)target = value;
}

static inline uint64_t mmio_read64(void* target)
{
    return *(volatile uint64_t*)target;
}

static inline uint32_t mmio_read32(void* target)
{
    return *(volatile uint32_t*)target;
}

static inline uint16_t mmio_read16(void* target)
{
    return *(volatile uint16_t*)target;
}

static inline uint8_t mmio_read8(void* target)
{
    return *(volatile uint8_t*)target;
}
