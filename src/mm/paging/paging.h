#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../../system14.h"
#include "../../limine.h"

/*
    * paging.h
    * struct and other definitions for paging code
    * copyright (C) 2023 DanielH
    * resources used:
        * Intel SDM Vol 3A
        * osdev.org
        * help from klange, Qwinci, dr micheal morbius on osdev discord. thanks guys!
*/

/* x86_64 Page-Table Entry */
/* See Intel SDM, Vol 3A */
struct PTE
{
    uint64_t Present : 1; // Must be 1 to be allowed
    uint64_t RW : 1;
    uint64_t UserSupervisor : 1;
    uint64_t PLWriteThrough : 1;
    uint64_t PLCacheDisable : 1;
    uint64_t Accessed : 1;
    uint64_t Ignored : 1;
    uint64_t PageSize : 1; // We use 4KiB pages so this must be ignored.
    uint64_t Ignored2 : 3;
    uint64_t Ignored3 : 1;
    uint64_t PhysAddr : 40;
    uint64_t Reserved : 12;
}__attribute__((packed));

typedef enum
{
    _4K = 0,
    _2M = 1,
} PageSizes;

/* Page Table */
struct PT
{
    struct PTE values[512];
}__attribute__((aligned(0x1000)));

void InitializePaging(struct limine_memmap_response mmap, uint64_t hhdm_base, uint64_t kernelSize, uint64_t kernelPhysBase, uint64_t kernelVirtBase, uintptr_t fbBase, uintptr_t fbSize);
extern void cr3load(uint64_t cr3);
KSTATUS MapMemory(struct PT* pml4, uint64_t virt, uint64_t phys, PageSizes size, bool user);
struct PT* CreateProcessPML4(void* start, void* end, bool inKernelCode);
void LoadKernelPML4();
